#include "CoreMinimal.h"
#if ENGINE_MAJOR_VERSION >= 5
#include "Misc/AutomationTest.h"
#include "UE427AssetPath.h"
#include "Misc/CommandLine.h"
#include "Misc/FileHelper.h"
#include "Misc/Parse.h"
#include "Animation/AnimBlueprint.h"
#include "Animation/AnimBlueprintGeneratedClass.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimInstanceProxy.h"
#include "Animation/AnimNode_TransitionResult.h"
#include "Animation/AnimNode_SequencePlayer.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Serialization/JsonSerializer.h"
#include "Dom/JsonObject.h"

#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRecoveredAnimationRoundTrip, "AssetGenerator.Graphs.RecoveredAnimationRoundTrip", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRecoveredAnimationRoundTrip::RunTest(const FString& Parameters)
{
    FString Path, Text;
    if (!FParse::Value(FCommandLine::Get(), TEXT("AnimationRecoveryJson="), Path)) { AddInfo(TEXT("Supply AnimationRecoveryJson to audit a recovered asset.")); return true; }
    TSharedPtr<FJsonObject> Json;
    if (!FFileHelper::LoadFileToString(Text, *Path) || !FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Text), Json)) { AddError(TEXT("Cannot read recovery JSON")); return false; }
    const FString Package = Json->GetStringField(TEXT("AssetPackage"));
    UAnimBlueprint* Blueprint = LoadObject<UAnimBlueprint>(nullptr, *(Package + TEXT(".") + Json->GetStringField(TEXT("AssetName"))));
    if (!TestNotNull(TEXT("Reload recovered blueprint"), Blueprint)) return false;
    FKismetEditorUtilities::CompileBlueprint(Blueprint);
    if (!TestTrue(TEXT("Reloaded asset recompiles"), Blueprint->Status != BS_Error)) return false;
    auto Class = CastChecked<UAnimBlueprintGeneratedClass>(Blueprint->GeneratedClass);
    const auto Data = Json->GetObjectField(TEXT("AssetSerializedData"))->GetObjectField(TEXT("AnimationBlueprintData"));
    const auto& SourceNodes = Data->GetArrayField(TEXT("Nodes"));
    int32 ExpectedNodes = SourceNodes.Num();
    TSet<int32> TransitionDelegates;
    for (const auto& Machine : Data->GetArrayField(TEXT("StateMachines")))
        for (const auto& State : Machine->AsObject()->GetArrayField(TEXT("States")))
            for (const auto& Rule : State->AsObject()->GetArrayField(TEXT("Transitions")))
            {
                const int32 Index = Rule->AsObject()->GetIntegerField(TEXT("CanTakeDelegateIndex"));
                if (TransitionDelegates.Contains(Index)) ++ExpectedNodes;
                TransitionDelegates.Add(Index);
            }
    TestEqual(TEXT("Compiled animation node count including shared transition clones"), Class->GetAnimNodeProperties().Num(), ExpectedNodes);
    const auto& Machines = Class->GetBakedStateMachines();
    TestEqual(TEXT("State machine count"), Machines.Num(), Data->GetArrayField(TEXT("StateMachines")).Num());
    auto Mesh = NewObject<USkeletalMeshComponent>();
    auto Instance = NewObject<UAnimInstance>(Mesh, Class);
    FAnimInstanceProxy Proxy(Instance);
    FAnimationUpdateContext Context(&Proxy, 0.0f);
    TArray<FName> BoolNames;
    TFunction<void(const TSharedPtr<FJsonValue>&)> CollectInputs = [&](const TSharedPtr<FJsonValue>& Value)
    {
        if (Value->Type == EJson::Object)
        {
            FString Instruction, Name;
            const auto Object = Value->AsObject();
            if (Object->TryGetStringField(TEXT("Inst"), Instruction) && Instruction == TEXT("InstanceVariable") && Object->TryGetStringField(TEXT("VariableName"), Name))
                BoolNames.AddUnique(FName(*Name));
            for (const auto& Pair : Object->Values) CollectInputs(Pair.Value);
        }
        else if (Value->Type == EJson::Array)
            for (const auto& Item : Value->AsArray()) CollectInputs(Item);
    };
    for (const auto& Node : SourceNodes)
        if (Node->AsObject()->GetStringField(TEXT("Type")) == TEXT("AnimNode_TransitionResult"))
            CollectInputs(Node->AsObject()->TryGetField(TEXT("InputExpressions")));
    for (const FName Name : BoolNames)
        if (!FindFProperty<FBoolProperty>(Class, Name)) { AddError(TEXT("Non-boolean transition input: ") + Name.ToString()); return false; }
    if (BoolNames.Num() > 12) { AddError(TEXT("Truth table exceeds 12 boolean variables")); return false; }
    TFunction<bool(const TSharedPtr<FJsonObject>&)> Evaluate = [&](const TSharedPtr<FJsonObject>& Expression)
    {
        const FString Inst = Expression->GetStringField(TEXT("Inst"));
        if (Inst == TEXT("True")) return true;
        if (Inst == TEXT("False")) return false;
        if (Inst == TEXT("InstanceVariable"))
        {
            auto Property = FindFProperty<FBoolProperty>(Class, *Expression->GetStringField(TEXT("VariableName")));
            if (!Property) { AddError(TEXT("Missing boolean input")); return false; }
            return Property->GetPropertyValue_InContainer(Instance);
        }
        if (Inst == TEXT("CallMath"))
        {
            const auto& Args = Expression->GetArrayField(TEXT("Parameters"));
            const FString Function = Expression->GetStringField(TEXT("Function"));
            if (Function == TEXT("Not_PreBool")) return !Evaluate(Args[0]->AsObject());
            if (Function == TEXT("BooleanAND")) return Evaluate(Args[0]->AsObject()) && Evaluate(Args[1]->AsObject());
            if (Function == TEXT("BooleanOR")) return Evaluate(Args[0]->AsObject()) || Evaluate(Args[1]->AsObject());
        }
        AddError(TEXT("Unsupported truth-table expression")); return false;
    };
    int32 Checks = 0;
    for (const auto& MachineValue : Data->GetArrayField(TEXT("StateMachines")))
    {
        auto Source = MachineValue->AsObject();
        const auto* Machine = Machines.FindByPredicate([&](const FBakedAnimationStateMachine& Value) { return Value.MachineName.ToString() == Source->GetStringField(TEXT("MachineName")); });
        if (!Machine) { AddError(TEXT("Missing recovered machine")); return false; }
        const auto& States = Source->GetArrayField(TEXT("States"));
        TestEqual(TEXT("State count"), Machine->States.Num(), States.Num());
        for (const auto& StateValue : States)
        {
            const auto State = StateValue->AsObject();
            const auto* Compiled = Machine->States.FindByPredicate([&](const FBakedAnimationState& Value) { return Value.StateName.ToString() == State->GetStringField(TEXT("StateName")); });
            if (!Compiled) { AddError(TEXT("Missing recovered state")); return false; }
            TestEqual(TEXT("Conduit flag"), Compiled->bIsAConduit, State->GetBoolField(TEXT("bIsAConduit")));
            const auto& Rules = State->GetArrayField(TEXT("Transitions"));
            if (!TestEqual(TEXT("Transition count"), Compiled->Transitions.Num(), Rules.Num())) return false;
            for (int32 RuleIndex = 0; RuleIndex < Rules.Num(); ++RuleIndex)
            {
                auto Rule = Rules[RuleIndex]->AsObject();
                const auto& ActualRule = Compiled->Transitions[RuleIndex];
                const auto& ActualEdge = Machine->Transitions[ActualRule.TransitionIndex];
                const auto ExpectedEdge = Source->GetArrayField(TEXT("Transitions"))[Rule->GetIntegerField(TEXT("TransitionIndex"))]->AsObject();
                TestEqual(TEXT("Transition destination and priority"), Machine->States[ActualEdge.NextState].StateName.ToString(), States[ExpectedEdge->GetIntegerField(TEXT("NextState"))]->AsObject()->GetStringField(TEXT("StateName")));
                TestEqual(TEXT("Transition duration"), ActualEdge.CrossfadeDuration, (float)ExpectedEdge->GetNumberField(TEXT("CrossfadeDuration")));
                const auto SourceNode = SourceNodes[SourceNodes.Num() - 1 - Rule->GetIntegerField(TEXT("CanTakeDelegateIndex"))]->AsObject();
                FStructProperty* Property = Class->GetAnimNodeProperties()[Class->GetAnimNodeProperties().Num() - 1 - ActualRule.CanTakeDelegateIndex];
                if (!TestEqual(TEXT("Transition runtime node type"), Property->Struct.Get(), FAnimNode_TransitionResult::StaticStruct())) return false;
                auto Runtime = Property->ContainerPtrToValuePtr<FAnimNode_TransitionResult>(Instance);
                for (int32 Mask = 0; Mask < (1 << BoolNames.Num()); ++Mask)
                {
                    for (int32 Bit = 0; Bit < BoolNames.Num(); ++Bit)
                        FindFProperty<FBoolProperty>(Class, BoolNames[Bit])->SetPropertyValue_InContainer(Instance, (Mask & (1 << Bit)) != 0);
                    const auto Inputs = SourceNode->GetObjectField(TEXT("InputExpressions"));
                    const bool Expected = Inputs->HasField(TEXT("bCanEnterTransition")) ? Evaluate(Inputs->GetObjectField(TEXT("bCanEnterTransition"))) : SourceNode->GetObjectField(TEXT("Properties"))->GetBoolField(TEXT("bCanEnterTransition"));
                    Runtime->GetEvaluateGraphExposedInputs().Execute(Context);
                    if (!TestEqual(TEXT("Compiled transition truth table"), Runtime->bCanEnterTransition, Expected)) return false;
                    ++Checks;
                }
            }
        }
    }
    AddInfo(FString::Printf(TEXT("Verified %d compiled transition evaluations against recovered expressions"), Checks));
    return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInheritedAnimationDefaults, "AssetGenerator.Graphs.InheritedAnimationDefaults", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FInheritedAnimationDefaults::RunTest(const FString& Parameters)
{
    FString Path, Text;
    if (!FParse::Value(FCommandLine::Get(), TEXT("AnimationRecoveryJson="), Path)) return false;
    TSharedPtr<FJsonObject> Json;
    if (!FFileHelper::LoadFileToString(Text, *Path) || !FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Text), Json)) return false;
    auto Blueprint = LoadObject<UAnimBlueprint>(nullptr, *(Json->GetStringField(TEXT("AssetPackage")) + TEXT(".") + Json->GetStringField(TEXT("AssetName"))));
    if (!TestNotNull(TEXT("Child animation blueprint"), Blueprint)) return false;
    const auto Data = Json->GetObjectField(TEXT("AssetSerializedData"))->GetObjectField(TEXT("AnimationBlueprintData"));
    const TSharedPtr<FJsonObject>* Defaults = nullptr;
    for (const auto& Export : Data->GetArrayField(TEXT("Exports")))
        if (Export->AsObject()->GetStringField(TEXT("Name")).StartsWith(TEXT("Default__"))) Export->AsObject()->TryGetObjectField(TEXT("Properties"), Defaults);
    if (!Defaults) return false;
    for (int32 Pass = 0; Pass < 2; ++Pass)
    {
        if (Pass) FKismetEditorUtilities::CompileBlueprint(Blueprint);
        auto CDO = Blueprint->GeneratedClass->GetDefaultObject();
        for (const auto& Pair : (*Defaults)->Values)
        {
            if (Pair.Value->Type != EJson::Object) continue;
            FString ObjectPath, ObjectName;
            auto Reference = Pair.Value->AsObject();
            if (!Reference->TryGetStringField(TEXT("ObjectPath"), ObjectPath) || !Reference->TryGetStringField(TEXT("ObjectName"), ObjectName)) continue;
            FString Package, Index, Type, Name;
            if (!ObjectPath.Split(TEXT("."), &Package, &Index, ESearchCase::CaseSensitive, ESearchDir::FromEnd) || !ObjectName.Split(TEXT("'"), &Type, &Name)) continue;
            Name.RemoveFromEnd(TEXT("'"));
            auto Property = FindFProperty<FObjectPropertyBase>(Blueprint->GeneratedClass, *Pair.Key);
            if (!TestNotNull(TEXT("Inherited property ") + Pair.Key, Property)) continue;
            auto Actual = Property->GetObjectPropertyValue_InContainer(CDO);
            TestEqual(FString::Printf(TEXT("Pass %d %s"), Pass, *Pair.Key), Actual ? Actual->GetPathName() : TEXT("None"), Package + TEXT(".") + Name);
        }
        auto Mesh = NewObject<USkeletalMeshComponent>();
        auto Instance = NewObject<UAnimInstance>(Mesh, Blueprint->GeneratedClass);
        FAnimInstanceProxy Proxy(Instance);
        FAnimationUpdateContext Context(&Proxy, 0.0f);
        int32 Players = 0;
        auto Class = CastChecked<UAnimBlueprintGeneratedClass>(Blueprint->GeneratedClass);
        for (auto Property : Class->GetAnimNodeProperties())
        {
            if (!Property->Struct->IsChildOf(FAnimNode_SequencePlayer::StaticStruct())) continue;
            auto Player = Property->ContainerPtrToValuePtr<FAnimNode_SequencePlayer>(Instance);
            Player->GetEvaluateGraphExposedInputs().Execute(Context);
            TestNotNull(TEXT("Inherited sequence player ") + Property->GetName(), Player->GetSequence());
            AddInfo(Property->GetName() + TEXT(" -> ") + (Player->GetSequence() ? Player->GetSequence()->GetPathName() : TEXT("None")));
            ++Players;
        }
        TestTrue(TEXT("Inherited sequence players found"), Players > 0);
    }
    return !HasAnyErrors();
}
#endif

#endif
