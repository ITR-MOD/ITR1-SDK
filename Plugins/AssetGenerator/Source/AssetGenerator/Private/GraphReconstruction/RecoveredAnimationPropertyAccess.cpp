#include "AnimationGraphReconstruction.h"
#include "UE427AssetPath.h"
#include "Animation/AnimBlueprint.h"
#include "K2Node_VariableGet.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Dom/JsonObject.h"
#include "Modules/ModuleManager.h"
#include "UObject/UnrealType.h"

namespace AnimationGraphReconstruction
{
bool RestorePropertyAccesses(UAnimBlueprint* Blueprint, const TSharedPtr<FJsonObject>& Data, FString& Error)
{
    TArray<UEdGraph*> Graphs;
    Blueprint->GetAllGraphs(Graphs);
    UClass* AccessClass = nullptr;
    for (const auto& CopyValue : Data->GetArrayField(TEXT("PropertyCopies")))
    {
        const auto Copy = CopyValue->AsObject();
        const auto& Destination = Copy->GetArrayField(TEXT("Destination"));
        if (Destination.Num() != 1 || !Destination[0]->AsString().StartsWith(TEXT("K2Node_PropertyAccess"))) continue;
        const FString VariableName = Destination[0]->AsString();
        for (auto Graph : Graphs)
        {
            const auto OldNodes = Graph->Nodes;
            for (auto OldNode : OldNodes)
            {
                auto Getter = Cast<UK2Node_VariableGet>(OldNode);
                if (!Getter || Getter->VariableReference.GetMemberName() != FName(*VariableName)) continue;
                if (!AccessClass)
                {
                    FModuleManager::Get().LoadModule(TEXT("PropertyAccessNode"));
                    AccessClass = LoadObject<UClass>(nullptr, TEXT("/Script/PropertyAccessNode.K2Node_PropertyAccess"));
                }
                if (!AccessClass) { Error = TEXT("PropertyAccessNode editor module is unavailable."); return false; }
                auto Access = NewObject<UEdGraphNode>(Graph, AccessClass);
                Access->CreateNewGuid();
                Graph->AddNode(Access, false, false);
                auto PathProperty = FindFProperty<FArrayProperty>(AccessClass, TEXT("Path"));
                auto ContextProperty = FindFProperty<FNameProperty>(AccessClass, TEXT("ContextId"));
                if (!PathProperty || !ContextProperty) { Error = TEXT("Property access node schema changed."); return false; }
                FScriptArrayHelper Path(PathProperty, PathProperty->ContainerPtrToValuePtr<void>(Access));
                const auto& Source = Copy->GetArrayField(TEXT("Source"));
                Path.Resize(Source.Num());
                for (int32 Index = 0; Index < Source.Num(); ++Index)
                    *reinterpret_cast<FString*>(Path.GetRawPtr(Index)) = Source[Index]->AsString();
                const TCHAR* Contexts[] = {TEXT("UnBatched_ThreadSafe"), TEXT("Batched_WorkerThreadPreEventGraph"), TEXT("Batched_WorkerThreadPostEventGraph"), TEXT("Batched_GameThreadPreEventGraph"), TEXT("Batched_GameThreadPostEventGraph")};
                const int32 Batch = Copy->GetIntegerField(TEXT("BatchIndex"));
                if (Batch < 0 || Batch >= UE_ARRAY_COUNT(Contexts)) { Error = TEXT("Unknown animation property access batch."); return false; }
                ContextProperty->SetPropertyValue_InContainer(Access, FName(Contexts[Batch]));
                Access->AllocateDefaultPins();
                auto Output = Access->FindPin(TEXT("Value"));
                auto OldOutput = Getter->GetValuePin();
                if (!Output || !OldOutput) { Error = TEXT("Could not restore property access output: ") + VariableName; return false; }
                TFunction<bool(UEdGraphPin*, UEdGraphPin*)> Reconnect = [&](UEdGraphPin* Old, UEdGraphPin* New)
                {
                    if (!UE427IsEmpty(Old->SubPins))
                    {
                        Graph->GetSchema()->SplitPin(New, false);
                        for (auto Child : Old->SubPins)
                        {
                            const FString Suffix = Child->PinName.ToString().RightChop(Old->PinName.ToString().Len());
                            auto* Match = New->SubPins.FindByPredicate([&](const UEdGraphPin* Candidate) { return Candidate->PinName.ToString() == New->PinName.ToString() + Suffix; });
                            if (!Match || !Reconnect(Child, *Match)) return false;
                        }
                    }
                    const auto Targets = Old->LinkedTo;
                    Old->BreakAllPinLinks();
                    for (auto Target : Targets) if (!Graph->GetSchema()->TryCreateConnection(New, Target)) return false;
                    return true;
                };
                if (!Reconnect(OldOutput, Output)) { Error = TEXT("Cannot reconnect property access: ") + VariableName; return false; }
                Access->NodePosX = Getter->NodePosX;
                Access->NodePosY = Getter->NodePosY;
                Graph->RemoveNode(Getter);
            }
        }
    }
    return true;
}
}
