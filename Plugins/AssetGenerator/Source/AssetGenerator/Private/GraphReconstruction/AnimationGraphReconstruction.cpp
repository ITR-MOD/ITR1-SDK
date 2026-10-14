#include "AnimationGraphReconstruction.h"
#include "UE427AssetPath.h"
#include "Animation/AnimBlueprint.h"
#include "AnimationGraph.h"
#include "AnimationGraphSchema.h"
#include "AnimGraphNode_Root.h"
#include "AnimGraphNode_Slot.h"
#include "AnimGraphNode_LinkedInputPose.h"
#include "AnimGraphNode_SequenceEvaluator.h"
#include "AnimGraphNode_SequencePlayer.h"
#include "Animation/AnimSequenceBase.h"
#include "Dom/JsonObject.h"
#include "JsonObjectConverter.h"
#include "K2Node_VariableGet.h"
#include "K2Node_FunctionEntry.h"
#include "Kismet2/BlueprintEditorUtils.h"

namespace AnimationGraphReconstruction
{
bool Validate(const TSharedPtr<FJsonObject>& Data, FString& Error)
{
    if (Data && Data->HasField(TEXT("InputRecoveryVersion"))) return ValidateRecovered(Data, Error);
    const TArray<TSharedPtr<FJsonValue>>* Nodes = nullptr;
    int32 Version = 0;
    if (!Data || !Data->TryGetNumberField(TEXT("Version"), Version) || Version != 1 || !Data->TryGetArrayField(TEXT("Nodes"), Nodes))
    {
        Error = TEXT("Re-export this animation Blueprint with CAS to include AnimationBlueprintData version 1.");
        return false;
    }
    int32 Roots = 0;
    const TArray<TSharedPtr<FJsonValue>>* Copies = nullptr;
    if (Data->TryGetArrayField(TEXT("PropertyCopies"), Copies))
        for (const auto& Copy : *Copies)
        {
            const auto& Source = Copy->AsObject()->GetArrayField(TEXT("Source"));
            const auto& Destination = Copy->AsObject()->GetArrayField(TEXT("Destination"));
            const bool MutableInput = Source.Num() == 1 && Destination.Num() == 2 && Destination[0]->AsString() == TEXT("__AnimBlueprintMutables");
            const bool SequenceLength = Source.Num() == 2 && Source[1]->AsString() == TEXT("SequenceLength") && Destination.Num() == 1;
            if (!MutableInput && !SequenceLength)
            {
                Error = TEXT("Animation property-copy pattern requires additional reconstruction support.");
                return false;
            }
        }
    const TArray<TSharedPtr<FJsonValue>>* UnsupportedPaths;
    if (Data->TryGetArrayField(TEXT("UnsupportedPropertyPaths"), UnsupportedPaths) && !UE427IsEmpty(*UnsupportedPaths))
    {
        Error = TEXT("Animation property access uses indexed or function paths that require additional reconstruction support.");
        return false;
    }
    for (const auto& Value : *Nodes)
    {
        const auto Node = Value->AsObject();
        FString Type;
        const TSharedPtr<FJsonObject>* Properties = nullptr;
        const TSharedPtr<FJsonObject>* Bindings = nullptr;
        if (!Node || !Node->TryGetStringField(TEXT("Type"), Type) || !Node->TryGetObjectField(TEXT("Properties"), Properties) || !Node->TryGetObjectField(TEXT("Bindings"), Bindings))
        {
            Error = TEXT("Incomplete recovered animation node data.");
            return false;
        }
        if (Type == TEXT("AnimNode_Root")) ++Roots;
        else if (Type != TEXT("AnimNode_SequenceEvaluator") && Type != TEXT("AnimNode_SequencePlayer") && Type != TEXT("AnimNode_LinkedInputPose") && Type != TEXT("AnimNode_Slot"))
        {
            Error = TEXT("Animation graph reconstruction does not yet support ") + Type + TEXT(". The cooked data was retained; no placeholder was substituted.");
            return false;
        }
        for (const auto& Binding : (*Bindings)->Values)
        {
            if (Binding.Value->Type != EJson::Array || Binding.Value->AsArray().Num() != 1 || Binding.Value->AsArray()[0]->Type != EJson::String)
            {
                Error = TEXT("Animation node binding requires an unsupported property path: ") + Binding.Key;
                return false;
            }
        }
        for (const auto& Property : (*Properties)->Values)
        {
            if (Property.Value->Type != EJson::Object) continue;
            if (Property.Value->AsObject()->HasField(TEXT("ObjectPath")) && !(Property.Key == TEXT("Sequence") && (Type == TEXT("AnimNode_SequencePlayer") || Type == TEXT("AnimNode_SequenceEvaluator"))))
            {
                Error = TEXT("Constant animation asset references require additional reconstruction support; variable-bound evaluator inputs are supported.");
                return false;
            }
            int32 Link = INDEX_NONE;
            if (Property.Value->AsObject()->TryGetNumberField(TEXT("LinkID"), Link) && Link != INDEX_NONE && !Nodes->IsValidIndex(Link))
            {
                Error = TEXT("Recovered animation pose link references a missing node.");
                return false;
            }
            FString ClassName;
            if (Property.Value->AsObject()->TryGetStringField(TEXT("ClassName"), ClassName) && !UE427IsEmpty(ClassName) && ClassName != TEXT("None"))
            {
                Error = TEXT("External animation node callbacks require additional reconstruction support.");
                return false;
            }
        }
    }
    if (!UE427IsEmpty(*Nodes) && Roots != 1)
    {
        Error = TEXT("Animation reconstruction requires exactly one root graph.");
        return false;
    }
    return true;
}

bool Build(UAnimBlueprint* Blueprint, const TSharedPtr<FJsonObject>& Data, FString& Error)
{
    if (Data && Data->HasField(TEXT("InputRecoveryVersion"))) return BuildRecovered(Blueprint, Data, Error);
    if (!Validate(Data, Error)) return false;
    const auto& SourceNodes = Data->GetArrayField(TEXT("Nodes"));
    if (UE427IsEmpty(SourceNodes)) return true;
    UEdGraph* Graph = nullptr;
    for (UEdGraph* Existing : Blueprint->FunctionGraphs)
        if (Existing->GetFName() == TEXT("AnimGraph")) Graph = Existing;
    if (!Graph)
    {
        Graph = FBlueprintEditorUtils::CreateNewGraph(Blueprint, TEXT("AnimGraph"), UAnimationGraph::StaticClass(), UAnimationGraphSchema::StaticClass());
        Blueprint->FunctionGraphs.Add(Graph);
    }
    Graph->Modify();
    for (UEdGraphNode* Old : TArray<UEdGraphNode*>(Graph->Nodes))
    {
        Old->BreakAllNodeLinks();
        Graph->RemoveNode(Old);
    }
    TArray<UAnimGraphNode_Base*> Nodes;
    for (const auto& Value : SourceNodes)
    {
        const auto Source = Value->AsObject();
        const FString Type = Source->GetStringField(TEXT("Type"));
        UAnimGraphNode_Base* Node = nullptr;
        UScriptStruct* Struct = nullptr;
        void* Memory = nullptr;
        if (Type == TEXT("AnimNode_Root"))
        {
            auto Root = NewObject<UAnimGraphNode_Root>(Graph);
            Node = Root; Struct = FAnimNode_Root::StaticStruct(); Memory = &Root->Node;
        }
        else if (Type == TEXT("AnimNode_Slot"))
        {
            auto Slot = NewObject<UAnimGraphNode_Slot>(Graph);
            Node = Slot; Struct = FAnimNode_Slot::StaticStruct(); Memory = &Slot->Node;
        }
        else if (Type == TEXT("AnimNode_SequenceEvaluator"))
        {
            auto Evaluator = NewObject<UAnimGraphNode_SequenceEvaluator>(Graph);
            Node = Evaluator; Struct = FAnimNode_SequenceEvaluator::StaticStruct(); Memory = &Evaluator->Node;
        }
        else if (Type == TEXT("AnimNode_SequencePlayer"))
        {
            auto Player = NewObject<UAnimGraphNode_SequencePlayer>(Graph);
            Node = Player; Struct = FAnimNode_SequencePlayer::StaticStruct(); Memory = &Player->Node;
        }
        else
        {
            auto Input = NewObject<UAnimGraphNode_LinkedInputPose>(Graph);
            Node = Input; Struct = FAnimNode_LinkedInputPose::StaticStruct(); Memory = &Input->Node;
        }
        Node->CreateNewGuid();
        Graph->AddNode(Node, false, false);
        auto Properties = MakeShared<FJsonObject>();
        for (const auto& Pair : Source->GetObjectField(TEXT("Properties"))->Values)
        {
            if (Pair.Key == TEXT("Sequence") && Pair.Value->Type == EJson::Object && Pair.Value->AsObject()->HasField(TEXT("ObjectPath")))
            {
                const auto Reference = Pair.Value->AsObject();
                FString Package, ExportIndex;
                FString ClassName, ObjectName;
                if (!Reference->GetStringField(TEXT("ObjectPath")).Split(TEXT("."), &Package, &ExportIndex, ESearchCase::CaseSensitive, ESearchDir::FromEnd)
                    || !Reference->GetStringField(TEXT("ObjectName")).Split(TEXT("'"), &ClassName, &ObjectName))
                {
                    Error = TEXT("Invalid recovered animation sequence reference."); return false;
                }
                ObjectName.RemoveFromEnd(TEXT("'"));
                const FString Path = Package + TEXT(".") + ObjectName;
                if (!LoadObject<UAnimSequenceBase>(nullptr, *Path))
                {
                    Error = TEXT("Cannot load recovered animation sequence ") + Path; return false;
                }
                Properties->SetStringField(Pair.Key, Path);
                continue;
            }
            if (Pair.Key == TEXT("InitialUpdateFunction") || Pair.Key == TEXT("BecomeRelevantFunction") || Pair.Key == TEXT("UpdateFunction"))
            {
                FString Function;
                if (Pair.Value->AsObject()->TryGetStringField(TEXT("FunctionName"), Function) && Function != TEXT("None"))
                {
#if ENGINE_MAJOR_VERSION >= 5
                    auto& Reference = Pair.Key == TEXT("UpdateFunction") ? Node->UpdateFunction : Pair.Key == TEXT("InitialUpdateFunction") ? Node->InitialUpdateFunction : Node->BecomeRelevantFunction;
                    Reference.SetSelfMember(*Function);
#endif
                }
            }
            else if (FProperty* Property = Struct->FindPropertyByName(*Pair.Key))
            {
                if (const auto Structure = CastField<FStructProperty>(Property))
                    if (Structure->Struct->GetFName() == TEXT("PoseLink")) continue;
                Properties->SetField(Pair.Key, Pair.Value);
            }
        }
        if (!FJsonObjectConverter::JsonObjectToUStruct(Properties, Struct, Memory, 0, 0))
        {
            Error = TEXT("Cannot restore animation node properties for ") + Type;
            return false;
        }
        Node->AllocateDefaultPins();
        for (auto& Pin : Node->ShowPinForProperties)
            if (Source->GetObjectField(TEXT("Bindings"))->HasField(Pin.PropertyName.ToString())) Pin.bShowPin = true;
        Node->ReconstructNode();
        Node->NodePosX = -Nodes.Num() * 300;
        for (const auto& Binding : Source->GetObjectField(TEXT("Bindings"))->Values)
        {
            auto Variable = NewObject<UK2Node_VariableGet>(Graph);
            Variable->VariableReference.SetSelfMember(*Binding.Value->AsArray()[0]->AsString());
            Variable->CreateNewGuid();
            Graph->AddNode(Variable, false, false);
            Variable->AllocateDefaultPins();
            Variable->NodePosX = Node->NodePosX - 280;
            Variable->NodePosY = 160 + Nodes.Num() * 100;
            UEdGraphPin* Target = Node->FindPin(*Binding.Key);
            if (!Target || !Graph->GetSchema()->TryCreateConnection(Variable->GetValuePin(), Target))
            {
                Error = TEXT("Cannot connect animation input ") + Binding.Key;
                return false;
            }
        }
        Nodes.Add(Node);
#if ENGINE_MAJOR_VERSION >= 5
        for (const FMemberReference* Reference : {&Node->InitialUpdateFunction, &Node->BecomeRelevantFunction, &Node->UpdateFunction})
            if (Reference->GetMemberName() != NAME_None)
                for (UEdGraph* Function : Blueprint->FunctionGraphs)
                    if (Function->GetFName() == Reference->GetMemberName())
                        for (UEdGraphNode* Entry : Function->Nodes)
                            if (auto FunctionEntry = Cast<UK2Node_FunctionEntry>(Entry)) Function(void)0;
#endif
    }
    for (int32 Index = 0; Index < Nodes.Num(); ++Index)
    {
        for (const auto& Pair : SourceNodes[Index]->AsObject()->GetObjectField(TEXT("Properties"))->Values)
        {
            if (Pair.Value->Type != EJson::Object) continue;
            int32 Link;
            if (!Pair.Value->AsObject()->TryGetNumberField(TEXT("LinkID"), Link) || Link == INDEX_NONE) continue;
            UEdGraphPin* Target = Nodes[Index]->FindPin(*Pair.Key);
            UEdGraphPin* Output = nullptr;
            if (Nodes.IsValidIndex(Link))
                for (UEdGraphPin* Pin : Nodes[Link]->Pins)
                    if (Pin->Direction == EGPD_Output && Pin->PinType.PinCategory == TEXT("struct")) { Output = Pin; break; }
            if (!Target || !Output || !Graph->GetSchema()->TryCreateConnection(Output, Target))
            {
                Error = TEXT("Cannot restore cooked pose link ") + Pair.Key;
                return false;
            }
        }
    }
    FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
    return true;
}
}
