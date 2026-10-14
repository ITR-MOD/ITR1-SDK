#include "AnimationGraphReconstruction.h"
#include "UE427AssetPath.h"
#include "K2Node_BreakStruct.h"
#include "Curves/CurveFloat.h"
#include "Animation/BlendProfile.h"
#include "UObject/UObjectHash.h"
#include "ControlRigGraphReconstruction.h"
#include "Animation/AnimBlueprint.h"
#include "AnimationGraph.h"
#include "AnimationGraphSchema.h"
#include "AnimationStateMachineGraph.h"
#include "AnimationStateGraph.h"
#include "AnimationTransitionGraph.h"
#include "AnimGraphNode_StateResult.h"
#include "AnimGraphNode_TransitionResult.h"
#include "AnimGraphNode_Base.h"
#include "AnimGraphNode_StateMachine.h"
#include "AnimGraphNode_SaveCachedPose.h"
#include "AnimGraphNode_UseCachedPose.h"
#include "AnimGraphNode_LinkedAnimLayer.h"
#include "AnimStateNode.h"
#include "AnimStateConduitNode.h"
#include "AnimStateTransitionNode.h"
#include "AnimStateEntryNode.h"
#include "EdGraphSchema_K2.h"
#include "K2Node_VariableGet.h"
#include "K2Node_CallFunction.h"
#include "K2Node_FunctionEntry.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "JsonObjectConverter.h"
#include "Dom/JsonObject.h"
#include "UObject/UnrealType.h"
#include "UObject/UObjectIterator.h"
#include "Misc/AutomationTest.h"
#include "Serialization/JsonSerializer.h"

namespace AnimationGraphReconstruction
{
namespace
{
using FObject = TSharedPtr<FJsonObject>;

TSharedPtr<FJsonValue> RestoreObjectReferences(const TSharedPtr<FJsonValue>& Value, FString& Error)
{
    if (Value->Type == EJson::Array)
    {
        TArray<TSharedPtr<FJsonValue>> Values;
        for (const auto& Item : Value->AsArray())
        {
            auto Restored = RestoreObjectReferences(Item, Error);
            if (!Restored) return nullptr;
            Values.Add(Restored);
        }
        return MakeShared<FJsonValueArray>(Values);
    }
    if (Value->Type != EJson::Object) return Value;
    const auto Object = Value->AsObject();
    if (Object->HasField(TEXT("ObjectPath")) && Object->HasField(TEXT("ObjectName")))
    {
        FString Package, Export, Type, Name;
        if (!Object->GetStringField(TEXT("ObjectPath")).Split(TEXT("."), &Package, &Export, ESearchCase::CaseSensitive, ESearchDir::FromEnd) ||
            !Object->GetStringField(TEXT("ObjectName")).Split(TEXT("'"), &Type, &Name))
        { Error = TEXT("Invalid nested animation reference."); return nullptr; }
        Name.RemoveFromEnd(TEXT("'"));
        const FString Path = Package + TEXT(".") + Name;
        UObject* Resolved = LoadObject<UObject>(nullptr, *Path);
        if (!Resolved)
        {
            if (UPackage* ObjectPackage = LoadPackage(nullptr, *Package, LOAD_None))
            {
                TArray<UObject*> PackageObjects;
                GetObjectsWithPackage(ObjectPackage, PackageObjects, true);
                for (UObject* Candidate : PackageObjects)
                    if (Candidate->GetFName() == FName(*Name) && Candidate->GetClass()->GetName() == Type) { Resolved = Candidate; break; }
            }
        }
        if (!Resolved) { Error = TEXT("Missing nested animation object: ") + Path; return nullptr; }
        return MakeShared<FJsonValueString>(Resolved->GetPathName());

    }
    auto Result = MakeShared<FJsonObject>();
    for (const auto& Pair : Object->Values)
    {
        auto Restored = RestoreObjectReferences(Pair.Value, Error);
        if (!Restored) return nullptr;
        Result->SetField(Pair.Key, Restored);
    }
    return MakeShared<FJsonValueObject>(Result);
}

FString EditorClass(const FObject& Node)
{
    FString Type = Node->GetStringField(TEXT("Type"));
    Type.RemoveFromStart(TEXT("AnimNode_"));
    if (Type == TEXT("ConvertLocalToComponentSpace")) Type = TEXT("LocalToComponentSpace");
    if (Type == TEXT("ConvertComponentToLocalSpace")) Type = TEXT("ComponentToLocalSpace");
    if (Type == TEXT("ControlRig")) return TEXT("/Script/ControlRigDeveloper.AnimGraphNode_ControlRig");
    if (Type == TEXT("RefPose")) Type = Node->GetObjectField(TEXT("Properties"))->GetStringField(TEXT("RefPoseType")).EndsWith(TEXT("EIT_Additive")) ? TEXT("IdentityPose") : TEXT("LocalRefPose");
    const FString StandardPath = TEXT("/Script/AnimGraph.AnimGraphNode_") + Type;
    if (LoadObject<UClass>(nullptr, *StandardPath)) return StandardPath;
    for (TObjectIterator<UClass> It; It; ++It)
    {
        if (!It->IsChildOf(UAnimGraphNode_Base::StaticClass()) || It->HasAnyClassFlags(CLASS_Abstract)) continue;
        const FStructProperty* Property = FindFProperty<FStructProperty>(*It, TEXT("Node"));
        if (Property && Property->Struct->GetName() == Node->GetStringField(TEXT("Type"))) return It->GetPathName();
    }
    return StandardPath;
}

FString PinName(FString Name)
{
    return Name.Replace(TEXT("["), TEXT("_")).Replace(TEXT("]"), TEXT(""));
}

void ClearGraph(UEdGraph* Graph)
{
    for (UEdGraphNode* Node : TArray<UEdGraphNode*>(Graph->Nodes))
    {
        Node->BreakAllNodeLinks();
        Graph->RemoveNode(Node);
    }
    Graph->SubGraphs.Empty();
}

template<class T> T* AddNode(UEdGraph* Graph)
{
    T* Node = NewObject<T>(Graph);
    Node->CreateNewGuid();
    Graph->AddNode(Node, false, false);
    Node->PostPlacedNewNode();
    Node->AllocateDefaultPins();
    return Node;
}

void Links(const FObject& Properties, TFunctionRef<void(const FString&, int32)> Visit)
{
    for (const auto& Pair : Properties->Values)
    {
        int32 Link;
        if (Pair.Value->Type == EJson::Object && Pair.Value->AsObject()->TryGetNumberField(TEXT("LinkID"), Link)) Visit(Pair.Key, Link);
        if (Pair.Value->Type == EJson::Array)
        {
            int32 Index = 0;
            for (const auto& Value : Pair.Value->AsArray())
            {
                if (Value->Type == EJson::Object && Value->AsObject()->TryGetNumberField(TEXT("LinkID"), Link)) Visit(FString::Printf(TEXT("%s_%d"), *Pair.Key, Index), Link);
                ++Index;
            }
        }
    }
}

UClass* ExpressionObjectClass(const FObject& Expression, UClass* Owner)
{
    const FString Inst = Expression->GetStringField(TEXT("Inst"));
    if (Inst == TEXT("Context")) return ExpressionObjectClass(Expression->GetObjectField(TEXT("Expression")), ExpressionObjectClass(Expression->GetObjectField(TEXT("Context")), Owner));
    if (!Owner) return nullptr;
    FProperty* Property = nullptr;
    if (Inst == TEXT("InstanceVariable")) Property = Owner->FindPropertyByName(*Expression->GetStringField(TEXT("VariableName")));
    if (Inst == TEXT("FinalFunction") || Inst == TEXT("VirtualFunction") || Inst == TEXT("LocalFinalFunction") || Inst == TEXT("LocalVirtualFunction"))
    {
        FString Name;
        if (!Expression->TryGetStringField(TEXT("Function"), Name)) Expression->TryGetStringField(TEXT("FunctionName"), Name);
        UFunction* Function = Owner->FindFunctionByName(*Name);
        Property = Function ? Function->GetReturnProperty() : nullptr;
    }
    if (auto Object = CastField<FObjectPropertyBase>(Property)) return Object->PropertyClass;
    return nullptr;
}

FProperty* ExpressionProperty(const FObject& Expression, UClass* Owner)
{
    const FString Inst = Expression->GetStringField(TEXT("Inst"));
    if (Inst == TEXT("Context")) return ExpressionProperty(Expression->GetObjectField(TEXT("Expression")), ExpressionObjectClass(Expression->GetObjectField(TEXT("Context")), Owner));
    if (!Owner) return nullptr;
    if (Inst == TEXT("InstanceVariable")) return Owner->FindPropertyByName(*Expression->GetStringField(TEXT("VariableName")));
    if (Inst == TEXT("FinalFunction") || Inst == TEXT("VirtualFunction") || Inst == TEXT("LocalFinalFunction") || Inst == TEXT("LocalVirtualFunction") || Inst == TEXT("CallMath") || Inst == TEXT("PureFunctionOutput"))
    {
        FString ClassPath, Name;
        if (Expression->TryGetStringField(TEXT("ContextClass"), ClassPath) || Expression->TryGetStringField(TEXT("FunctionOwner"), ClassPath)) Owner = LoadObject<UClass>(nullptr, *ClassPath);
        if (!Expression->TryGetStringField(TEXT("Function"), Name)) Expression->TryGetStringField(TEXT("FunctionName"), Name);
        UFunction* Function = Owner ? Owner->FindFunctionByName(*Name) : nullptr;
        return Function ? Function->GetReturnProperty() : nullptr;
    }
    if (Inst == TEXT("PropertyAccessPath"))
    {
        UStruct* Scope = Owner;
        FProperty* Property = nullptr;
        for (const auto& Segment : Expression->GetArrayField(TEXT("Path")))
        {
            Property = Scope ? FindFProperty<FProperty>(Scope, *Segment->AsString()) : nullptr;
            if (!Property)
                if (auto Class = Cast<UClass>(Scope))
                    if (UFunction* Function = Class->FindFunctionByName(*Segment->AsString())) Property = Function->GetReturnProperty();
            if (auto Object = CastField<FObjectPropertyBase>(Property)) Scope = Object->PropertyClass;
            else if (auto Struct = CastField<FStructProperty>(Property)) Scope = Struct->Struct;
            else Scope = nullptr;
        }
        return Property;
    }
    if (Inst == TEXT("StructMemberContext"))
    {
        auto Struct = CastField<FStructProperty>(ExpressionProperty(Expression->GetObjectField(TEXT("StructExpression")), Owner));
        return Struct ? Struct->Struct->FindPropertyByName(*Expression->GetStringField(TEXT("PropertyName"))) : nullptr;
    }
    return nullptr;
}

bool ConnectExpression(UEdGraph* Graph, const FObject& Expression, UEdGraphPin* Target, FString& Error)
{
    if (!Target) { Error = TEXT("Recovered animation input pin is missing."); return false; }
    const FString Inst = Expression->GetStringField(TEXT("Inst"));
    UEdGraphPin* Output = nullptr;
    if (Inst == TEXT("PropertyAccessPath"))
    {
        UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForGraph(Graph);
        UStruct* Scope = Blueprint ? Blueprint->SkeletonGeneratedClass.Get() : nullptr;
        for (const auto& Segment : Expression->GetArrayField(TEXT("Path")))
        {
            const FName Name(*Segment->AsString());
            FProperty* Property = Scope ? FindFProperty<FProperty>(Scope, *Name.ToString()) : nullptr;
            UEdGraphPin* NextOutput = nullptr;
            if (auto Class = Cast<UClass>(Scope))
            {
                if (Property)
                {
                    auto Variable = NewObject<UK2Node_VariableGet>(Graph);
                    if (Output) Variable->VariableReference.SetExternalMember(Name, Class);
                    else Variable->VariableReference.SetSelfMember(Name);
                    Variable->CreateNewGuid(); Graph->AddNode(Variable, false, false); Variable->AllocateDefaultPins();
                    if (Output && !Graph->GetSchema()->TryCreateConnection(Output, Variable->FindPin(UEdGraphSchema_K2::PN_Self))) { Error = TEXT("Cannot connect property path object."); return false; }
                    NextOutput = Variable->GetValuePin();
                }
                else
                {
                    UFunction* Function = Class->FindFunctionByName(Name);
                    if (!Function || !Function->HasAnyFunctionFlags(FUNC_BlueprintPure)) { Error = TEXT("Unresolved pure property path function: ") + Name.ToString(); return false; }
                    for (TFieldIterator<FProperty> It(Function); It && It->HasAnyPropertyFlags(CPF_Parm); ++It)
                        if (!It->HasAnyPropertyFlags(CPF_ReturnParm)) { Error = TEXT("Property path function requires arguments."); return false; }
                    auto Call = NewObject<UK2Node_CallFunction>(Graph);
                    Call->SetFromFunction(Function); Call->CreateNewGuid(); Graph->AddNode(Call, false, false); Call->AllocateDefaultPins();
                    if (Output && !Graph->GetSchema()->TryCreateConnection(Output, Call->FindPin(UEdGraphSchema_K2::PN_Self))) { Error = TEXT("Cannot connect property path function object."); return false; }
                    NextOutput = Call->GetReturnValuePin();
                    Property = Function->GetReturnProperty();
                }
            }
            else if (auto Struct = Cast<UScriptStruct>(Scope); Struct && Property && Output)
            {
                auto Break = NewObject<UK2Node_BreakStruct>(Graph);
                Break->StructType = Struct; Break->CreateNewGuid(); Graph->AddNode(Break, false, false); Break->AllocateDefaultPins();
                UEdGraphPin* Input = nullptr;
                for (UEdGraphPin* Pin : Break->Pins) if (Pin && Pin->Direction == EGPD_Input && Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct) Input = Pin;
                if (!Input || !Graph->GetSchema()->TryCreateConnection(Output, Input)) { Error = TEXT("Cannot connect property path struct."); return false; }
                NextOutput = Break->FindPin(Name, EGPD_Output);
            }
            if (!NextOutput) { Error = TEXT("Unresolved animation property path segment: ") + Name.ToString(); return false; }
            Output = NextOutput;
            if (auto Object = CastField<FObjectPropertyBase>(Property)) Scope = Object->PropertyClass;
            else if (auto Struct = CastField<FStructProperty>(Property)) Scope = Struct->Struct;
            else Scope = nullptr;
        }
    }
    else if (Inst == TEXT("StructMemberContext"))
    {
        UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForGraph(Graph);
        auto Struct = CastField<FStructProperty>(ExpressionProperty(Expression->GetObjectField(TEXT("StructExpression")), Blueprint ? Blueprint->SkeletonGeneratedClass.Get() : nullptr));
        if (!Struct) { Error = TEXT("Cannot resolve recovered struct member scope."); return false; }
        auto Break = NewObject<UK2Node_BreakStruct>(Graph);
        Break->StructType = Struct->Struct;
        Graph->AddNode(Break, false, false); Break->CreateNewGuid(); Break->AllocateDefaultPins();
        UEdGraphPin* Input = nullptr;
        for (auto Pin : Break->Pins) if (Pin->Direction == EGPD_Input && Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct) { Input = Pin; break; }
        if (!ConnectExpression(Graph, Expression->GetObjectField(TEXT("StructExpression")), Input, Error)) return false;
        Output = Break->FindPin(*Expression->GetStringField(TEXT("PropertyName")), EGPD_Output);
    }
    else if (Inst == TEXT("StructConst") && Expression->GetStringField(TEXT("Struct")) == TEXT("/Script/GameplayTags.GameplayTag"))
    {
        FString Tag;
        for (const auto& Pair : Expression->GetObjectField(TEXT("Properties"))->Values)
            for (const auto& Value : Pair.Value->AsArray())
                if (Value->AsObject()->GetStringField(TEXT("Inst")) == TEXT("NameConst")) Tag = Value->AsObject()->GetStringField(TEXT("Value"));
        UClass* Library = LoadObject<UClass>(nullptr, TEXT("/Script/GameplayTags.BlueprintGameplayTagLibrary"));
        UFunction* Function = Library ? Library->FindFunctionByName(TEXT("MakeLiteralGameplayTag")) : nullptr;
        if (!Function) { Error = TEXT("Gameplay tag literal function is unavailable."); return false; }
        auto Literal = NewObject<UK2Node_CallFunction>(Graph);
        Literal->SetFromFunction(Function); Graph->AddNode(Literal, false, false); Literal->CreateNewGuid(); Literal->AllocateDefaultPins();
        auto ValuePin = Literal->FindPin(TEXT("Value"));
        if (!ValuePin) { Error = TEXT("Gameplay tag literal input is unavailable."); return false; }
        Graph->GetSchema()->TrySetDefaultValue(*ValuePin, FString::Printf(TEXT("(TagName=\"%s\")"), *Tag));
        Output = Literal->GetReturnValuePin();
    }
    else if (Inst == TEXT("Context"))
    {
        auto Blueprint = FBlueprintEditorUtils::FindBlueprintForGraph(Graph);
        UClass* Owner = ExpressionObjectClass(Expression->GetObjectField(TEXT("Context")), Blueprint ? Blueprint->SkeletonGeneratedClass.Get() : nullptr);
        const auto Inner = Expression->GetObjectField(TEXT("Expression"));
        const FString InnerInst = Inner->GetStringField(TEXT("Inst"));
        UEdGraphPin* Self = nullptr;
        if (Owner && InnerInst == TEXT("InstanceVariable"))
        {
            auto Variable = NewObject<UK2Node_VariableGet>(Graph);
            Variable->VariableReference.SetExternalMember(*Inner->GetStringField(TEXT("VariableName")), Owner);
            Variable->CreateNewGuid(); Graph->AddNode(Variable, false, false); Variable->AllocateDefaultPins();
            Output = Variable->GetValuePin(); Self = Variable->FindPin(UEdGraphSchema_K2::PN_Self);
        }
        else if (Owner && (InnerInst == TEXT("VirtualFunction") || InnerInst == TEXT("FinalFunction")))
        {
            auto Function = Owner->FindFunctionByName(*Inner->GetStringField(TEXT("Function")));
            if (!Function || !Function->HasAnyFunctionFlags(FUNC_BlueprintPure)) { Error = TEXT("Unresolved pure context function."); return false; }
            auto Call = NewObject<UK2Node_CallFunction>(Graph);
            Call->SetFromFunction(Function); Call->CreateNewGuid(); Graph->AddNode(Call, false, false); Call->AllocateDefaultPins();
            Self = Call->FindPin(UEdGraphSchema_K2::PN_Self); Output = Call->GetReturnValuePin();
            int32 ParameterIndex = 0;
            const auto& Parameters = Inner->GetArrayField(TEXT("Parameters"));
            for (TFieldIterator<FProperty> It(Function); It && It->HasAnyPropertyFlags(CPF_Parm); ++It)
                if (!It->HasAnyPropertyFlags(CPF_ReturnParm) && (!It->HasAnyPropertyFlags(CPF_OutParm) || It->HasAnyPropertyFlags(CPF_ReferenceParm | CPF_ConstParm)))
                    if (!Parameters.IsValidIndex(ParameterIndex) || !ConnectExpression(Graph, Parameters[ParameterIndex++]->AsObject(), Call->FindPin(It->GetFName()), Error)) return false;
            if (ParameterIndex != Parameters.Num()) { Error = TEXT("Context function parameter count mismatch."); return false; }
        }
        else { Error = TEXT("Unsupported recovered object context."); return false; }
        if (!ConnectExpression(Graph, Expression->GetObjectField(TEXT("Context")), Self, Error)) return false;
    }
    else if (Inst == TEXT("InstanceVariable"))
    {
        auto Variable = NewObject<UK2Node_VariableGet>(Graph);
        Variable->VariableReference.SetSelfMember(*Expression->GetStringField(TEXT("VariableName")));
        Variable->CreateNewGuid(); Graph->AddNode(Variable, false, false); Variable->AllocateDefaultPins();
        Output = Variable->GetValuePin();
    }
    else if (Inst == TEXT("PrimitiveCast"))
    {
        const FString Cast = Expression->GetStringField(TEXT("CastType"));
        if (Cast != TEXT("FloatToDouble") && Cast != TEXT("DoubleToFloat")) { Error = TEXT("Unsupported animation cast ") + Cast; return false; }
        auto Conversion = MakeShared<FJsonObject>();
        Conversion->SetStringField(TEXT("Inst"), TEXT("CallMath"));
        Conversion->SetStringField(TEXT("Function"), TEXT("Conv_") + Cast);
        Conversion->SetStringField(TEXT("ContextClass"), TEXT("/Script/Engine.KismetMathLibrary"));
        Conversion->SetArrayField(TEXT("Parameters"), {MakeShared<FJsonValueObject>(Expression->GetObjectField(TEXT("Expression")))});
        return ConnectExpression(Graph, Conversion, Target, Error);
    }
    else if (Inst == TEXT("CallMath") || Inst == TEXT("PureFunctionOutput") || Inst == TEXT("LocalVirtualFunction") || Inst == TEXT("LocalFinalFunction") || Inst == TEXT("FinalFunction") || Inst == TEXT("VirtualFunction"))
    {
        UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForGraph(Graph);
        UClass* Owner = !Expression->HasField(TEXT("ContextClass")) ? (Blueprint ? Blueprint->SkeletonGeneratedClass.Get() : nullptr) : LoadObject<UClass>(nullptr, *Expression->GetStringField(TEXT("ContextClass")));
        FString FunctionName;
        if (!Expression->TryGetStringField(TEXT("Function"), FunctionName)) Expression->TryGetStringField(TEXT("FunctionName"), FunctionName);
        UFunction* Function = Owner ? Owner->FindFunctionByName(*FunctionName) : nullptr;
        if (!Function || !Function->HasAnyFunctionFlags(FUNC_BlueprintPure)) { Error = TEXT("Animation expression is not a resolvable pure function."); return false; }
        auto Call = NewObject<UK2Node_CallFunction>(Graph);
        Call->SetFromFunction(Function); Call->CreateNewGuid(); Graph->AddNode(Call, false, false); Call->AllocateDefaultPins();
        const auto& Parameters = Expression->GetArrayField(TEXT("Parameters"));
        int32 Index = 0;
        for (TFieldIterator<FProperty> It(Function); It && It->HasAnyPropertyFlags(CPF_Parm); ++It)
        {
            if (It->HasAnyPropertyFlags(CPF_ReturnParm) || (It->HasAnyPropertyFlags(CPF_OutParm) && !It->HasAnyPropertyFlags(CPF_ReferenceParm | CPF_ConstParm))) continue;
            if (!Parameters.IsValidIndex(Index) || !ConnectExpression(Graph, Parameters[Index++]->AsObject(), Call->FindPin(It->GetFName()), Error)) return false;
        }
        if (Index != Parameters.Num()) { Error = TEXT("Animation expression parameter count differs from the cooked function."); return false; }
        Output = Inst == TEXT("PureFunctionOutput") ? Call->FindPin(*Expression->GetStringField(TEXT("Output")), EGPD_Output) : Call->GetReturnValuePin();
    }
    else if (Inst == TEXT("VectorConst"))
    {
        const FVector Value(Expression->GetNumberField(TEXT("X")), Expression->GetNumberField(TEXT("Y")), Expression->GetNumberField(TEXT("Z")));
        Graph->GetSchema()->TrySetDefaultValue(*Target, Value.ToString());
        return true;
    }
    else if (Inst == TEXT("True") || Inst == TEXT("False") || Inst == TEXT("FloatConst") || Inst == TEXT("DoubleConst") || Inst == TEXT("IntConst"))
    {
        FString Value = Inst == TEXT("True") ? TEXT("true") : TEXT("false");
        if (Inst.EndsWith(TEXT("Const"))) Value = FString::SanitizeFloat(Expression->GetNumberField(TEXT("Value")));
        Graph->GetSchema()->TrySetDefaultValue(*Target, Value);
        return true;
    }
    else { Error = TEXT("Unsupported recovered animation expression ") + Inst; return false; }
    if (!Output || !Graph->GetSchema()->TryCreateConnection(Output, Target)) { Error = TEXT("Cannot connect recovered animation expression to ") + Target->PinName.ToString(); return false; }
    Output->GetOwningNode()->NodePosX = Target->GetOwningNode()->NodePosX - 300;
    return true;
}

bool OrderStateMachines(const FObject& Data, TArray<int32>& Order, FString& Error)
{
    const auto& Nodes = Data->GetArrayField(TEXT("Nodes"));
    const auto& Machines = Data->GetArrayField(TEXT("StateMachines"));
    TMap<int32, int32> Parents;
    TArray<int32> MachineNodes;
    for (int32 Index = 0; Index < Nodes.Num(); ++Index)
    {
        const auto Node = Nodes[Index]->AsObject();
        if (Node->GetStringField(TEXT("Type")) != TEXT("AnimNode_StateMachine")) continue;
        MachineNodes.Add(Index);
        const int32 MachineIndex = Node->GetObjectField(TEXT("Properties"))->GetIntegerField(TEXT("StateMachineIndexInClass"));
        if (!Machines.IsValidIndex(MachineIndex)) { Error = TEXT("Missing baked state machine."); return false; }
        for (const auto& StateValue : Machines[MachineIndex]->AsObject()->GetArrayField(TEXT("States")))
        {
            const auto State = StateValue->AsObject();
            if (State->GetBoolField(TEXT("bIsAConduit"))) continue;
            TSet<int32> Visited;
            TFunction<bool(int32)> Visit = [&](int32 Child)
            {
                if (!Nodes.IsValidIndex(Child)) { Error = TEXT("Invalid state pose index."); return false; }
                if (Visited.Contains(Child)) return true;
                Visited.Add(Child);
                const auto ChildNode = Nodes[Child]->AsObject();
                const FString Type = ChildNode->GetStringField(TEXT("Type"));
                if (Type == TEXT("AnimNode_StateMachine"))
                {
                    if (Parents.Contains(Child) && Parents[Child] != Index) { Error = TEXT("State machine shared between different parent machines."); return false; }
                    Parents.Add(Child, Index);
                    return true;
                }
                if (Type == TEXT("AnimNode_UseCachedPose")) return true;
                bool Valid = true;
                Links(ChildNode->GetObjectField(TEXT("Properties")), [&](const FString&, int32 Link) { if (Link >= 0) Valid = Visit(Link) && Valid; });
                return Valid;
            };
            if (!Visit(Nodes.Num() - 1 - State->GetIntegerField(TEXT("StateRootNodeIndex")))) return false;
        }
    }
    TSet<int32> Visiting, Complete;
    TFunction<bool(int32)> Append = [&](int32 Index)
    {
        if (Complete.Contains(Index)) return true;
        if (Visiting.Contains(Index)) { Error = TEXT("Cyclic recovered state machine ownership."); return false; }
        Visiting.Add(Index);
        if (const int32* Parent = Parents.Find(Index)) if (!Append(*Parent)) return false;
        Visiting.Remove(Index); Complete.Add(Index); Order.Add(Index);
        return true;
    };
    for (int32 Index : MachineNodes) if (!Append(Index)) return false;
    return true;
}
}

#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRecoveredNestedMachineOrder, "AssetGenerator.Graphs.NestedAnimationMachineOrder", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRecoveredNestedMachineOrder::RunTest(const FString& Parameters)
{
    const FString Text = TEXT(R"({"Nodes":[
        {"Type":"AnimNode_StateMachine","Properties":{"StateMachineIndexInClass":0}},
        {"Type":"AnimNode_StateResult","Properties":{"Result":{"LinkID":-1}}},
        {"Type":"AnimNode_StateMachine","Properties":{"StateMachineIndexInClass":1}},
        {"Type":"AnimNode_StateResult","Properties":{"Result":{"LinkID":0}}}],
        "StateMachines":[{"States":[{"bIsAConduit":false,"StateRootNodeIndex":2}]},
        {"States":[{"bIsAConduit":false,"StateRootNodeIndex":0}]}]})");
    FObject Data;
    if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Text), Data)) return false;
    TArray<int32> Order;
    FString Error;
    TestTrue(TEXT("Nested machine ownership resolves"), OrderStateMachines(Data, Order, Error));
    TestEqual(TEXT("Both machines are scheduled"), Order.Num(), 2);
    if (Order.Num() == 2)
    {
        TestEqual(TEXT("Outer machine precedes nested machine regardless of serialized order"), Order[0], 2);
        TestEqual(TEXT("Nested machine follows its parent"), Order[1], 0);
    }
    Data->GetArrayField(TEXT("Nodes"))[1]->AsObject()->GetObjectField(TEXT("Properties"))->GetObjectField(TEXT("Result"))->SetNumberField(TEXT("LinkID"), 2);
    Order.Reset();
    TestFalse(TEXT("Cyclic machine ownership is rejected"), OrderStateMachines(Data, Order, Error));
    return true;
}
#endif

bool ValidateRecovered(const FObject& Data, FString& Error)
{
    if (Data->GetIntegerField(TEXT("InputRecoveryVersion")) != 1) { Error = TEXT("Unsupported animation input recovery version."); return false; }
    if (!UE427IsEmpty(Data->GetArrayField(TEXT("InputRecoveryErrors")))) { Error = Data->GetArrayField(TEXT("InputRecoveryErrors"))[0]->AsString(); return false; }
    for (const auto& PathValue : Data->GetArrayField(TEXT("UnsupportedPropertyPaths")))
    {
        bool Found = false;
        for (const auto& CopyValue : Data->GetArrayField(TEXT("PropertyCopies")))
        {
            const auto Copy = CopyValue->AsObject();
            TArray<FString> Segments;
            for (const auto& Segment : Copy->GetArrayField(TEXT("Source"))) Segments.Add(Segment->AsString());
            const auto& Destination = Copy->GetArrayField(TEXT("Destination"));
            if (FString::Join(Segments, TEXT(".")) == PathValue->AsString() && Destination.Num() == 1 && Destination[0]->AsString().StartsWith(TEXT("K2Node_PropertyAccess"))) Found = true;
        }
        TFunction<void(const TSharedPtr<FJsonValue>&)> FindRecoveredPath = [&](const TSharedPtr<FJsonValue>& Value)
        {
            if (!Value) return;
            if (Value->Type == EJson::Array) { for (const auto& Child : Value->AsArray()) FindRecoveredPath(Child); return; }
            if (Value->Type != EJson::Object) return;
            const auto Object = Value->AsObject();
            FString Inst;
            if (Object->TryGetStringField(TEXT("Inst"), Inst) && Inst == TEXT("PropertyAccessPath"))
            {
                TArray<FString> Segments;
                for (const auto& Segment : Object->GetArrayField(TEXT("Path"))) Segments.Add(Segment->AsString());
                if (FString::Join(Segments, TEXT(".")) == PathValue->AsString()) Found = true;
            }
            for (const auto& Pair : Object->Values) FindRecoveredPath(Pair.Value);
        };
        FindRecoveredPath(Data->TryGetField(TEXT("Nodes")));
        if (!Found) { Error = TEXT("Unmapped animation property function path: ") + PathValue->AsString(); return false; }
    }
    const auto& Nodes = Data->GetArrayField(TEXT("Nodes"));
    int32 Roots = 0;
    for (const auto& Value : Nodes)
    {
        const auto Node = Value->AsObject();
        if (Node->GetStringField(TEXT("Type")) == TEXT("AnimNode_Root")) ++Roots;
        const FString NodeType = Node->GetStringField(TEXT("Type"));
        if (!LoadObject<UClass>(nullptr, *EditorClass(Node))) { Error = TEXT("Missing animation editor class ") + EditorClass(Node); return false; }
        bool Valid = true;
        Links(Node->GetObjectField(TEXT("Properties")), [&](const FString&, int32 Link) { if (Link != INDEX_NONE && !Nodes.IsValidIndex(Link)) Valid = false; });
        if (!Valid) { Error = TEXT("Invalid recovered pose link."); return false; }
    }
    if (!UE427IsEmpty(Nodes) && Roots == 0) { Error = TEXT("Recovery requires an animation root."); return false; }
    for (const auto& MachineValue : Data->GetArrayField(TEXT("StateMachines")))
    {
        const auto Machine = MachineValue->AsObject();
        for (const auto& TransitionValue : Machine->GetArrayField(TEXT("Transitions")))
        {
            const auto Transition = TransitionValue->AsObject();
            if (Transition->GetStringField(TEXT("LogicType")) == TEXT("ETransitionLogicType::TLT_Custom"))
            { Error = TEXT("Custom transition graphs need additional reconstruction support."); return false; }
        }
        for (const auto& StateValue : Machine->GetArrayField(TEXT("States")))
        {
            const auto State = StateValue->AsObject();
            if (State->GetBoolField(TEXT("bIsAConduit"))) continue;
            TSet<int32> Visited;
            TFunction<bool(int32)> Inspect = [&](int32 Index)
            {
                if (!Nodes.IsValidIndex(Index)) return false;
                if (Visited.Contains(Index)) return true;
                Visited.Add(Index);
                const auto Node = Nodes[Index]->AsObject();
                if (Node->GetStringField(TEXT("Type")) == TEXT("AnimNode_StateMachine")) return true;
                if (Node->GetStringField(TEXT("Type")) == TEXT("AnimNode_UseCachedPose")) return true;
                bool Valid = true;
                Links(Node->GetObjectField(TEXT("Properties")), [&](const FString&, int32 Link) { if (Link >= 0) Valid = Inspect(Link) && Valid; });
                return Valid;
            };
            if (!Inspect(Nodes.Num() - 1 - State->GetIntegerField(TEXT("StateRootNodeIndex"))))
            { Error = TEXT("Invalid recovered state root."); return false; }
        }
    }
    TArray<int32> MachineOrder;
    return OrderStateMachines(Data, MachineOrder, Error);
}

bool BuildRecovered(UAnimBlueprint* Blueprint, const FObject& Data, FString& Error)
{
    if (!ValidateRecovered(Data, Error)) return false;
    auto Source = Data->GetArrayField(TEXT("Nodes"));
    const int32 CookedNodeCount = Source.Num();
    if (UE427IsEmpty(Source)) return true;
    UEdGraph* Graph = nullptr;
    for (UEdGraph* Candidate : Blueprint->FunctionGraphs) if (Candidate->GetFName() == TEXT("AnimGraph")) Graph = Candidate;
    if (!Graph)
    {
        Graph = FBlueprintEditorUtils::CreateNewGraph(Blueprint, TEXT("AnimGraph"), UAnimationGraph::StaticClass(), UAnimationGraphSchema::StaticClass());
        Blueprint->FunctionGraphs.Add(Graph);
    }
    ClearGraph(Graph);
    if (!ControlRigGraphReconstruction::BuildAssets(Data, Error)) return false;
    TArray<UEdGraph*> Owners; Owners.Init(Graph, Source.Num());
    TArray<UAnimGraphNode_Base*> Nodes; Nodes.Init(nullptr, Source.Num());
    for (int32 Index = 0; Index < Source.Num(); ++Index)
        if (Source[Index]->AsObject()->GetStringField(TEXT("Type")) == TEXT("AnimNode_Root"))
        {
            const FString Name = Source[Index]->AsObject()->GetObjectField(TEXT("Properties"))->GetStringField(TEXT("Name"));
            UEdGraph* Owner = Graph;
            if (Name != TEXT("AnimGraph"))
            {
                Owner = nullptr;
                for (UEdGraph* Candidate : Blueprint->FunctionGraphs) if (Candidate->GetName() == Name) Owner = Candidate;
                for (const FBPInterfaceDescription& Interface : Blueprint->ImplementedInterfaces)
                    for (UEdGraph* Candidate : Interface.Graphs)
                        if (Candidate && Candidate->GetName() == Name) Owner = Candidate;
                if (Owner && !Owner->Schema->IsChildOf(UAnimationGraphSchema::StaticClass()))
                {
                    UEdGraph* PreviousOwner = Owner;
                    for (FBPInterfaceDescription& Interface : Blueprint->ImplementedInterfaces)
                        Interface.Graphs.Remove(PreviousOwner);
                    ClearGraph(PreviousOwner);
                    FBlueprintEditorUtils::RemoveGraph(Blueprint, PreviousOwner, EGraphRemoveFlags::MarkTransient);

                    Owner = nullptr;
                }
                if (!Owner) { Owner = FBlueprintEditorUtils::CreateNewGraph(Blueprint, *Name, UAnimationGraph::StaticClass(), UAnimationGraphSchema::StaticClass()); Blueprint->FunctionGraphs.Add(Owner); }
                for (FBPInterfaceDescription& Interface : Blueprint->ImplementedInterfaces)
                    if (Interface.Interface && Interface.Interface->FindFunctionByName(*Name))
                    {
                        Interface.Graphs.AddUnique(Owner);
                        Blueprint->FunctionGraphs.Remove(Owner);
                    }
                ClearGraph(Owner);
            }
            Owners[Index] = Owner;
            auto Root = NewObject<UAnimGraphNode_Base>(Owner, LoadObject<UClass>(nullptr, *EditorClass(Source[Index]->AsObject())));
            Root->CreateNewGuid(); Owner->AddNode(Root, false, false); Root->PostPlacedNewNode(); Root->AllocateDefaultPins(); Nodes[Index] = Root;
        }
    TSet<int32> Assigned;
    TFunction<bool(int32, UEdGraph*)> Assign = [&](int32 Index, UEdGraph* Owner)
    {
        if (!Source.IsValidIndex(Index)) { Error = TEXT("Invalid recovered state node index."); return false; }
        if (Assigned.Contains(Index))
        {
            if (Owners[Index] == Owner) return true;
            if (Source[Index]->AsObject()->GetStringField(TEXT("Type")) != TEXT("AnimNode_TransitionResult"))
            { Error = TEXT("Pose nodes shared across state graphs require cloning."); return false; }
            const auto Shared = Source[Index];
            Index = Source.Add(Shared);
            Owners.Add(Owner);
            Nodes.Add(nullptr);
        }
        Assigned.Add(Index); Owners[Index] = Owner;
        const auto Node = Source[Index]->AsObject();
        if (Node->GetStringField(TEXT("Type")) == TEXT("AnimNode_UseCachedPose")) return true;
        bool Valid = true;
        Links(Node->GetObjectField(TEXT("Properties")), [&](const FString&, int32 Link) { if (Link >= 0) Valid = Assign(Link, Owner) && Valid; });
        return Valid;
    };
    for (int32 Index = 0; Index < CookedNodeCount; ++Index)
        if (Source[Index]->AsObject()->GetStringField(TEXT("Type")) == TEXT("AnimNode_Root") && !Assign(Index, Owners[Index])) return false;
    const auto& Machines = Data->GetArrayField(TEXT("StateMachines"));
    TArray<int32> MachineOrder;
    if (!OrderStateMachines(Data, MachineOrder, Error)) return false;
    for (int32 Index : MachineOrder)
    {
        const auto Json = Source[Index]->AsObject();
        if (Json->GetStringField(TEXT("Type")) != TEXT("AnimNode_StateMachine")) continue;
        const int32 MachineIndex = Json->GetObjectField(TEXT("Properties"))->GetIntegerField(TEXT("StateMachineIndexInClass"));
        if (!Machines.IsValidIndex(MachineIndex)) { Error = TEXT("Missing baked state machine."); return false; }
        const auto Machine = Machines[MachineIndex]->AsObject();
        auto Node = AddNode<UAnimGraphNode_StateMachine>(Owners[Index]); Nodes[Index] = Node;
        auto MachineGraph = Node->EditorStateMachineGraph;
        FBlueprintEditorUtils::RenameGraph(MachineGraph, Machine->GetStringField(TEXT("MachineName")));
        TArray<UAnimStateNodeBase*> States;
        const auto& BakedStates = Machine->GetArrayField(TEXT("States"));
        for (const auto& StateValue : BakedStates)
        {
            const auto State = StateValue->AsObject();
            UAnimStateNodeBase* StateNode;
            const bool Conduit = State->GetBoolField(TEXT("bIsAConduit"));
            if (Conduit) StateNode = AddNode<UAnimStateConduitNode>(MachineGraph);
            else
            {
                auto PoseState = AddNode<UAnimStateNode>(MachineGraph);
                PoseState->bAlwaysResetOnEntry = State->GetBoolField(TEXT("bAlwaysResetOnEntry"));
                StateNode = PoseState;
            }
            States.Add(StateNode);
            UEdGraph* Bound = StateNode->GetBoundGraph();
            FBlueprintEditorUtils::RenameGraph(Bound, State->GetStringField(TEXT("StateName")));
            ClearGraph(Bound);
            int32 RuntimeIndex = State->GetIntegerField(Conduit ? TEXT("EntryRuleNodeIndex") : TEXT("StateRootNodeIndex"));
            if (!Assign(CookedNodeCount - 1 - RuntimeIndex, Bound)) return false;
            for (const TCHAR* Field : {TEXT("StartNotify"), TEXT("EndNotify"), TEXT("FullyBlendedNotify")})
            {
                const int32 NotifyIndex = State->GetIntegerField(Field);
                if (NotifyIndex == INDEX_NONE) continue;
                FObject Generated;
                for (const auto& Export : Data->GetArrayField(TEXT("Exports")))
                    if (Export->AsObject()->GetStringField(TEXT("Type")) == TEXT("AnimBlueprintGeneratedClass")) Generated = Export->AsObject();
                const TArray<TSharedPtr<FJsonValue>>* Notifies = nullptr;
                if (!Generated || !Generated->GetObjectField(TEXT("Properties"))->TryGetArrayField(TEXT("AnimNotifies"), Notifies) || !Notifies->IsValidIndex(NotifyIndex) || Conduit)
                { Error = TEXT("Cannot resolve state notify."); return false; }
                const auto Notify = (*Notifies)[NotifyIndex]->AsObject();
                for (const TCHAR* ObjectField : {TEXT("Notify"), TEXT("NotifyStateClass")})
                    if (Notify->HasField(ObjectField) && Notify->TryGetField(ObjectField)->Type != EJson::Null) { Error = TEXT("Custom state notify requires object recovery."); return false; }
                auto PoseState = CastChecked<UAnimStateNode>(StateNode);
                FAnimNotifyEvent& Event = FString(Field) == TEXT("StartNotify") ? PoseState->StateEntered : FString(Field) == TEXT("EndNotify") ? PoseState->StateLeft : PoseState->StateFullyBlended;
                Event.NotifyName = *Notify->GetStringField(TEXT("NotifyName"));
                double Threshold;
                if (Notify->TryGetNumberField(TEXT("TriggerWeightThreshold"), Threshold)) Event.TriggerWeightThreshold = Threshold;
            }
        }
        const int32 Initial = Machine->GetIntegerField(TEXT("InitialState"));
        if (!States.IsValidIndex(Initial)) { Error = TEXT("Invalid initial animation state."); return false; }
        for (UEdGraphNode* Entry : MachineGraph->Nodes)
            if (Cast<UAnimStateEntryNode>(Entry)) Entry->Pins[0]->MakeLinkTo(States[Initial]->GetInputPin());
        for (int32 StateIndex = 0; StateIndex < BakedStates.Num(); ++StateIndex)
        {
            int32 Priority = 0;
            for (const auto& RuleValue : BakedStates[StateIndex]->AsObject()->GetArrayField(TEXT("Transitions")))
            {
                const auto Rule = RuleValue->AsObject();
                const int32 TransitionIndex = Rule->GetIntegerField(TEXT("TransitionIndex"));
                const auto& Transitions = Machine->GetArrayField(TEXT("Transitions"));
                if (!Transitions.IsValidIndex(TransitionIndex)) { Error = TEXT("Invalid baked transition index."); return false; }
                const auto Transition = Transitions[TransitionIndex]->AsObject();
                const int32 Next = Transition->GetIntegerField(TEXT("NextState"));
                if (!States.IsValidIndex(Next) || !Rule->GetBoolField(TEXT("bDesiredTransitionReturnValue")) || Rule->GetIntegerField(TEXT("CustomResultNodeIndex")) != INDEX_NONE)
                { Error = TEXT("Unsupported custom or inverted transition rule."); return false; }
                auto Edge = AddNode<UAnimStateTransitionNode>(MachineGraph);
                Edge->CreateConnections(States[StateIndex], States[Next]);
                Edge->PriorityOrder = ++Priority;
                Edge->CrossfadeDuration = Transition->GetNumberField(TEXT("CrossfadeDuration"));
                const int64 Logic = StaticEnum<ETransitionLogicType::Type>()->GetValueByNameString(Transition->GetStringField(TEXT("LogicType")));
                if (Logic == INDEX_NONE) { Error = TEXT("Unknown transition logic type."); return false; }
                Edge->LogicType = (ETransitionLogicType::Type)Logic;
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 7
                Edge->bAllowInertializationForSelfTransitions = Transition->GetBoolField(TEXT("bAllowInertializationForSelfTransitions"));
#endif
                for (const TCHAR* ObjectField : {TEXT("CustomCurve"), TEXT("BlendProfile")})
                {
                    const auto Reference = Transition->TryGetField(ObjectField);
                    if (!Reference || Reference->Type == EJson::Null) continue;
                    const auto Restored = RestoreObjectReferences(Reference, Error);
                    if (!Restored) return false;
                    UObject* Object = LoadObject<UObject>(nullptr, *Restored->AsString());
                    if (FString(ObjectField) == TEXT("CustomCurve")) Edge->CustomBlendCurve = Cast<UCurveFloat>(Object);
                    else
                    {
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 6
                        Edge->BlendProfileWrapper.SetSkeletonBlendProfile(Cast<UBlendProfile>(Object));
#else
                        Edge->BlendProfile = Cast<UBlendProfile>(Object);
#endif
                    }
                }
                for (const TCHAR* Field : {TEXT("StartNotify"), TEXT("EndNotify"), TEXT("InterruptNotify")})
                {
                    const int32 NotifyIndex = Transition->GetIntegerField(Field);
                    if (NotifyIndex == INDEX_NONE) continue;
                    FObject Generated;
                    for (const auto& Export : Data->GetArrayField(TEXT("Exports")))
                        if (Export->AsObject()->GetStringField(TEXT("Type")) == TEXT("AnimBlueprintGeneratedClass")) Generated = Export->AsObject();
                    const TArray<TSharedPtr<FJsonValue>>* Notifies = nullptr;
                    if (!Generated || !Generated->GetObjectField(TEXT("Properties"))->TryGetArrayField(TEXT("AnimNotifies"), Notifies) || !Notifies->IsValidIndex(NotifyIndex))
                    { Error = TEXT("Cannot resolve transition notify."); return false; }
                    const auto Notify = (*Notifies)[NotifyIndex]->AsObject();
                    for (const TCHAR* ObjectField : {TEXT("Notify"), TEXT("NotifyStateClass")})
                        if (Notify->HasField(ObjectField) && Notify->TryGetField(ObjectField)->Type != EJson::Null) { Error = TEXT("Custom transition notify requires object recovery."); return false; }
                    FAnimNotifyEvent& Event = FString(Field) == TEXT("StartNotify") ? Edge->TransitionStart : FString(Field) == TEXT("EndNotify") ? Edge->TransitionEnd : Edge->TransitionInterrupt;
                    Event.NotifyName = *Notify->GetStringField(TEXT("NotifyName"));
                    double Threshold;
                    if (Notify->TryGetNumberField(TEXT("TriggerWeightThreshold"), Threshold)) Event.TriggerWeightThreshold = Threshold;
                }
                FString Blend = Transition->GetStringField(TEXT("BlendMode"));
                int64 BlendValue = StaticEnum<EAlphaBlendOption>()->GetValueByNameString(Blend);
                if (BlendValue == INDEX_NONE) { Error = TEXT("Unknown transition blend mode ") + Blend; return false; }
                Edge->BlendMode = (EAlphaBlendOption)BlendValue;
                Edge->bAutomaticRuleBasedOnSequencePlayerInState = Rule->GetBoolField(TEXT("bAutomaticRemainingTimeRule"));
                ClearGraph(Edge->BoundGraph);
                if (!Assign(CookedNodeCount - 1 - Rule->GetIntegerField(TEXT("CanTakeDelegateIndex")), Edge->BoundGraph)) return false;
            }
        }
    }
    for (int32 Index = 0; Index < Source.Num(); ++Index)
        if (Source[Index]->AsObject()->GetStringField(TEXT("Type")) == TEXT("AnimNode_UseCachedPose"))
        {
            UEdGraph* Owner = Owners[Index];
            for (UObject* Outer = Owner; Outer; Outer = Outer->GetOuter())
                if (auto Candidate = Cast<UEdGraph>(Outer))
                    if (Blueprint->FunctionGraphs.Contains(Candidate) || Blueprint->ImplementedInterfaces.ContainsByPredicate([&](const FBPInterfaceDescription& Interface) { return Interface.Graphs.Contains(Candidate); })) { Owner = Candidate; break; }
            const int32 Cache = Source[Index]->AsObject()->GetObjectField(TEXT("Properties"))->GetObjectField(TEXT("LinkToCachingNode"))->GetIntegerField(TEXT("LinkID"));
            if (!Assign(Cache, Owner)) return false;
        }
    for (int32 Index = 0; Index < Source.Num(); ++Index)
    {
        const auto Json = Source[Index]->AsObject();
        UAnimGraphNode_Base* Node = Nodes[Index];
        if (!Node)
        {
            Node = NewObject<UAnimGraphNode_Base>(Owners[Index], LoadObject<UClass>(nullptr, *EditorClass(Json)));
            Node->CreateNewGuid(); Owners[Index]->AddNode(Node, false, false); Node->PostPlacedNewNode(); Nodes[Index] = Node;
        }
        FStructProperty* NodeProperty = nullptr;
        for (TFieldIterator<FStructProperty> It(Node->GetClass()); It; ++It)
            if (It->Struct->GetName() == Json->GetStringField(TEXT("Type"))) { NodeProperty = *It; break; }
        if (!NodeProperty) { Error = TEXT("Missing runtime animation node property: ") + Node->GetClass()->GetName(); return false; }
        auto Properties = MakeShared<FJsonObject>();
        for (const auto& Pair : Json->GetObjectField(TEXT("Properties"))->Values)
        {
            if (Pair.Key == TEXT("StateMachineIndexInClass") || Pair.Key == TEXT("StateIndex") || Pair.Key == TEXT("SkeletonGuid") || Pair.Key == TEXT("VirtualBoneGuid") || Pair.Key == TEXT("PerBoneBlendWeights")) continue;
            if (Json->GetStringField(TEXT("Type")) == TEXT("AnimNode_ControlRig") &&
                (Pair.Key == TEXT("ControlRig") || Pair.Key == TEXT("ControlRigPerClass") || Pair.Key == TEXT("TargetInstance") || Pair.Key == TEXT("SourcePropertyNames") || Pair.Key == TEXT("DestPropertyNames") || Pair.Key == TEXT("DefaultControlRigClass"))) continue;
            if (Pair.Key.EndsWith(TEXT("Function")))
            {
                FString Function;
                if (Pair.Value->Type == EJson::Object && Pair.Value->AsObject()->TryGetStringField(TEXT("FunctionName"), Function) && Function != TEXT("None"))
                {

                    FString Owner;
                    if (Pair.Value->AsObject()->TryGetStringField(TEXT("ClassName"), Owner) && !UE427IsEmpty(Owner) && Owner != TEXT("None"))
                    { Error = TEXT("External animation callbacks require additional recovery: ") + Function; return false; }
                    FStructProperty* ReferenceProperty = FindFProperty<FStructProperty>(Node->GetClass(), *Pair.Key);
                    if (ReferenceProperty && ReferenceProperty->Struct == FMemberReference::StaticStruct())
                        ReferenceProperty->ContainerPtrToValuePtr<FMemberReference>(Node)->SetSelfMember(*Function);
                    else if (Pair.Key == TEXT("Function") && Json->GetStringField(TEXT("Type")) == TEXT("AnimNode_CallFunction"))
                    {
                        auto GraphProperty = FindFProperty<FObjectPropertyBase>(Node->GetClass(), TEXT("InnerGraph"));
                        auto PrototypeProperty = FindFProperty<FObjectPropertyBase>(Node->GetClass(), TEXT("CallFunctionPrototype"));
                        if (!GraphProperty || !PrototypeProperty) { Error = TEXT("Missing animation call function editor properties."); return false; }
                        UEdGraph* Inner = FBlueprintEditorUtils::CreateNewGraph(Node, NAME_None, UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());
                        UK2Node_CallFunction* Prototype = NewObject<UK2Node_CallFunction>(Inner);
                        Inner->AddNode(Prototype, false, false);
                        Prototype->CreateNewGuid();
                        Prototype->FunctionReference.SetSelfMember(*Function);
                        Prototype->AllocateDefaultPins();
                        GraphProperty->SetObjectPropertyValue_InContainer(Node, Inner);
                        PrototypeProperty->SetObjectPropertyValue_InContainer(Node, Prototype);
                    }
                    else { Error = TEXT("Missing animation callback editor binding: ") + Pair.Key; return false; }
                    for (UEdGraph* FunctionGraph : Blueprint->FunctionGraphs)
                        if (FunctionGraph->GetFName() == FName(*Function))
                            for (UEdGraphNode* Entry : FunctionGraph->Nodes) (void)Entry;
                }
                continue;
            }
            FProperty* Property = NodeProperty->Struct->FindPropertyByName(*Pair.Key);
            if (!Property) continue;
            if (CastField<FMapProperty>(Property) && Pair.Value->Type == EJson::Array && UE427IsEmpty(Pair.Value->AsArray()))
            { Properties->SetObjectField(Pair.Key, MakeShared<FJsonObject>()); continue; }
            if (Pair.Value->Type == EJson::Object && Pair.Value->AsObject()->HasField(TEXT("LinkID"))) continue;
            if (Pair.Value->Type == EJson::Array && !UE427IsEmpty(Pair.Value->AsArray()) && Pair.Value->AsArray()[0]->Type == EJson::Object && Pair.Value->AsArray()[0]->AsObject()->HasField(TEXT("LinkID")))
            {
                TArray<TSharedPtr<FJsonValue>> Poses;
                for (const auto& Unused : Pair.Value->AsArray()) Poses.Add(MakeShared<FJsonValueObject>(MakeShared<FJsonObject>()));
                Properties->SetArrayField(Pair.Key, Poses); continue;
            }
            if (Pair.Value->Type == EJson::Object && Pair.Value->AsObject()->HasField(TEXT("ObjectPath")))
            {
                const auto Object = Pair.Value->AsObject();
                FString Package, Export, Type, Name;
                if (!Object->GetStringField(TEXT("ObjectPath")).Split(TEXT("."), &Package, &Export, ESearchCase::CaseSensitive, ESearchDir::FromEnd)
                    || !Object->GetStringField(TEXT("ObjectName")).Split(TEXT("'"), &Type, &Name))
                { Error = TEXT("Invalid animation object reference: ") + Pair.Key; return false; }
                Name.RemoveFromEnd(TEXT("'"));
                const FString Path = Package + TEXT(".") + Name;
                if (!LoadObject<UObject>(nullptr, *Path)) { Error = TEXT("Missing animation object ") + Path; return false; }
                Properties->SetStringField(Pair.Key, Path); continue;
            }
            auto Restored = RestoreObjectReferences(Pair.Value, Error);
            if (!Restored) return false;
            Properties->SetField(Pair.Key, Restored);
        }
        if (!FJsonObjectConverter::JsonObjectToUStruct(Properties, NodeProperty->Struct, NodeProperty->ContainerPtrToValuePtr<void>(Node), 0, 0))
        { Error = TEXT("Cannot restore animation properties for ") + Json->GetStringField(TEXT("Name")); return false; }
        if (auto Save = Cast<UAnimGraphNode_SaveCachedPose>(Node)) Save->CacheName = Json->GetObjectField(TEXT("Properties"))->GetStringField(TEXT("CachePoseName"));
        if (Json->GetStringField(TEXT("Type")) == TEXT("AnimNode_BlendListByEnum"))
        {
            const auto Expression = Json->GetObjectField(TEXT("InputExpressions"))->GetObjectField(TEXT("ActiveEnumValue"));
            FProperty* Variable = ExpressionProperty(Expression, Blueprint->SkeletonGeneratedClass);
            UEnum* Enum = CastField<FByteProperty>(Variable) ? CastFieldChecked<FByteProperty>(Variable)->Enum : CastField<FEnumProperty>(Variable) ? CastFieldChecked<FEnumProperty>(Variable)->GetEnum() : nullptr;
            if (!Enum) { Error = TEXT("Cannot resolve blend-list enum."); return false; }
            FindFProperty<FObjectPropertyBase>(Node->GetClass(), TEXT("BoundEnum"))->SetObjectPropertyValue_InContainer(Node, Enum);
            auto Array = FindFProperty<FArrayProperty>(Node->GetClass(), TEXT("VisibleEnumEntries"));
            FScriptArrayHelper Entries(Array, Array->ContainerPtrToValuePtr<void>(Node));
            const auto& Mapping = Json->GetObjectField(TEXT("Properties"))->GetArrayField(TEXT("EnumToPoseIndex"));
            int32 MaxPose = 0; for (const auto& Value : Mapping) MaxPose = FMath::Max(MaxPose, (int32)Value->AsNumber());
            Entries.Resize(MaxPose);
            for (int32 Pose = 1; Pose <= MaxPose; ++Pose)
            {
                int32 EnumIndex = INDEX_NONE;
                for (int32 I = 0; I < Mapping.Num(); ++I) if ((int32)Mapping[I]->AsNumber() == Pose) { EnumIndex = I; break; }
                if (EnumIndex == INDEX_NONE) { Error = TEXT("Non-contiguous enum pose mapping."); return false; }
                *reinterpret_cast<FName*>(Entries.GetRawPtr(Pose - 1)) = Enum->GetNameByValue(EnumIndex);
            }
        }
        Node->AllocateDefaultPins();
        if (Json->GetStringField(TEXT("Type")) == TEXT("AnimNode_ControlRig"))
        {
            auto CustomPins = FindFProperty<FArrayProperty>(Node->GetClass(), TEXT("CustomPinProperties"));
            if (!CustomPins) { Error = TEXT("Missing Control Rig custom pin metadata."); return false; }
            FScriptArrayHelper Pins(CustomPins, CustomPins->ContainerPtrToValuePtr<void>(Node));
            for (int32 PinIndex = 0; PinIndex < Pins.Num(); ++PinIndex)
            {
                auto& Pin = *reinterpret_cast<FOptionalPinFromProperty*>(Pins.GetRawPtr(PinIndex));
                if (Json->GetObjectField(TEXT("InputExpressions"))->HasField(Pin.PropertyName.ToString())) Pin.bShowPin = true;
            }
        }
        if (auto StateGraph = Cast<UAnimationStateGraph>(Owners[Index]))
            if (auto Result = Cast<UAnimGraphNode_StateResult>(Node)) StateGraph->MyResultNode = Result;
        if (auto TransitionGraph = Cast<UAnimationTransitionGraph>(Owners[Index]))
            if (auto Result = Cast<UAnimGraphNode_TransitionResult>(Node)) TransitionGraph->MyResultNode = Result;
        for (auto& Pin : Node->ShowPinForProperties)
            for (const auto& Input : Json->GetObjectField(TEXT("InputExpressions"))->Values)
                if (Input.Key == Pin.PropertyName.ToString() || Input.Key.StartsWith(Pin.PropertyName.ToString() + TEXT("["))) Pin.bShowPin = true;
        Node->ReconstructNode();
        Node->NodePosX = -Index * 250;
    }
    FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
    for (auto Node : Nodes)
        if (auto Layer = Cast<UAnimGraphNode_LinkedAnimLayer>(Node))
        {
            auto ReferenceProperty = FindFProperty<FStructProperty>(Layer->GetClass(), TEXT("FunctionReference"));
            if (!ReferenceProperty) { Error = TEXT("Missing linked layer function reference."); return false; }
            auto Reference = ReferenceProperty->ContainerPtrToValuePtr<FMemberReference>(Layer);
            if (Layer->Node.Interface) Reference->SetExternalMember(Layer->Node.Layer, Layer->Node.Interface);
            else Reference->SetSelfMember(Layer->Node.Layer);
            Layer->ReconstructNode();
        }
    for (int32 Index = 0; Index < Source.Num(); ++Index)
    {
        auto Node = Nodes[Index];
        const auto Json = Source[Index]->AsObject();
        if (auto Use = Cast<UAnimGraphNode_UseCachedPose>(Node))
        {
            const int32 Link = Json->GetObjectField(TEXT("Properties"))->GetObjectField(TEXT("LinkToCachingNode"))->GetIntegerField(TEXT("LinkID"));
            auto Save = Nodes.IsValidIndex(Link) ? Cast<UAnimGraphNode_SaveCachedPose>(Nodes[Link]) : nullptr;
            if (!Save) { Error = TEXT("Missing cached pose producer."); return false; }
            Use->SaveCachedPoseNode = Save;
        }
        else
        {
            bool Valid = true;
            Links(Json->GetObjectField(TEXT("Properties")), [&](const FString& Pin, int32 Link)
            {
                if (Link == INDEX_NONE) return;
                UEdGraphPin* Output = nullptr;
                if (Nodes.IsValidIndex(Link)) for (UEdGraphPin* Candidate : Nodes[Link]->Pins)
                    if (Candidate->Direction == EGPD_Output && Candidate->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct) { Output = Candidate; break; }
                auto Input = Node->FindPin(*Pin);
                if (!Input && Pin.StartsWith(TEXT("InputPoses_")))
                {
                    const TArray<TSharedPtr<FJsonValue>>* Names = nullptr;
                    const int32 PoseIndex = FCString::Atoi(*Pin.Mid(11));
                    if (Json->GetObjectField(TEXT("Properties"))->TryGetArrayField(TEXT("InputPoseNames"), Names) && Names->IsValidIndex(PoseIndex))
                        Input = Node->FindPin(*(*Names)[PoseIndex]->AsString());
                }
                if (!Output || !Input || !Node->GetGraph()->GetSchema()->TryCreateConnection(Output, Input))
                {
                    FString Pins;
                    for (auto Candidate : Node->Pins) Pins += Candidate->PinName.ToString() + TEXT(" ");
                    Error = TEXT("Cannot connect animation pose ") + Json->GetStringField(TEXT("Name")) + TEXT(".") + Pin + TEXT(" (pins: ") + Pins + TEXT(")"); Valid = false;
                }
            });
            if (!Valid) return false;
        }
        for (const auto& Input : Json->GetObjectField(TEXT("InputExpressions"))->Values)
            if (!ConnectExpression(Node->GetGraph(), Input.Value->AsObject(), Node->FindPin(*PinName(Input.Key)), Error)) { Error = Json->GetStringField(TEXT("Name")) + TEXT(".") + Input.Key + TEXT(": ") + Error; return false; }
    }
    FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
    return true;
}
}
