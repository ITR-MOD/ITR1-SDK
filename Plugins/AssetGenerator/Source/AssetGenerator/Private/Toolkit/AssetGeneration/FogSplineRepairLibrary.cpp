#include "AssetGeneration/FogSplineRepairLibrary.h"
#include "UE427AssetPath.h"
#include "Engine/Blueprint.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraphSchema_K2.h"
#include "K2Node_MacroInstance.h"
#include "K2Node_FunctionResult.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
bool UFogSplineRepairLibrary::RepairResampleSpline(UBlueprint* Blueprint)
{
    if (!Blueprint || Blueprint->GetPathName() != TEXT("/Game/ITR2/BPs/Misc/Environment/Tools/BP_FogSpline.BP_FogSpline")) return false;
    for (auto Graph : Blueprint->FunctionGraphs)
    {
        if (!Graph || Graph->GetFName() != TEXT("ResampleSpline")) continue;
        UK2Node_MacroInstance* Loop = nullptr;
        UK2Node_FunctionResult* Result = nullptr;
        for (auto Node : Graph->Nodes)
        {
            if (auto Macro = Cast<UK2Node_MacroInstance>(Node))
                if (Macro->GetMacroGraph() && Macro->GetMacroGraph()->GetFName() == TEXT("ForLoop"))
                {
                    if (Loop) return false;
                    Loop = Macro;
                }
            if (auto Return = Cast<UK2Node_FunctionResult>(Node)) Result = Return;
        }
        if (!Loop || !Result) return false;
        auto Completed = Loop->FindPin(TEXT("Completed"));
        auto ReturnPin = Result->FindPin(UEdGraphSchema_K2::PN_Execute);
        if (!Completed || !ReturnPin) return false;
        Blueprint->Modify(); Graph->Modify(); Loop->Modify(); Result->Modify();
        Completed->BreakAllPinLinks();
        if (!Graph->GetSchema()->TryCreateConnection(Completed, ReturnPin)) return false;
        FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
        FKismetEditorUtilities::CompileBlueprint(Blueprint);
        return Blueprint->Status != BS_Error;
    }
    return false;
}
