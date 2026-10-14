#include "CoreMinimal.h"
#if ENGINE_MAJOR_VERSION >= 5
#include "ControlRigGraphReconstruction.h"
#include "UE427AssetPath.h"
#include "ControlRigBlueprint.h"
#include "ControlRigBlueprintFactory.h"
#include "RigVMModel/RigVMController.h"
#include "RigVMModel/RigVMGraph.h"
#include "Rigs/RigHierarchy.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Kismet2/CompilerResultsLog.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Misc/PackageName.h"
#include "UObject/SavePackage.h"
#include "Dom/JsonObject.h"

namespace ControlRigGraphReconstruction
{
bool BuildAssets(const TSharedPtr<FJsonObject>& AnimationData, FString& Error)
{
    const TArray<TSharedPtr<FJsonValue>>* Rigs = nullptr;
    if (!AnimationData->TryGetArrayField(TEXT("ControlRigs"), Rigs)) return true;
    for (const auto& Value : *Rigs)
    {
        const auto Rig = Value->AsObject();
        const auto& RecoveryErrors = Rig->GetArrayField(TEXT("RecoveryErrors"));
        if (!UE427IsEmpty(RecoveryErrors)) { Error = RecoveryErrors[0]->AsString(); return false; }
        const FString PackageName = Rig->GetStringField(TEXT("Package"));
        if (!FPackageName::IsValidLongPackageName(PackageName)) { Error = TEXT("Invalid recovered rig package: ") + PackageName; return false; }
        const FString Name = FPackageName::GetLongPackageAssetName(PackageName);
        UPackage* Package = CreatePackage(*PackageName);
        Package->FullyLoad();
        auto Blueprint = FindObject<UControlRigBlueprint>(Package, *Name);
        if (!Blueprint)
        {
            if (FindObject<UObject>(Package, *Name)) { Error = TEXT("Existing rig asset has an incompatible type: ") + PackageName; return false; }
            auto Factory = NewObject<UControlRigBlueprintFactory>();
            Blueprint = Cast<UControlRigBlueprint>(Factory->FactoryCreateNew(UControlRigBlueprint::StaticClass(), Package, *Name, RF_Public | RF_Standalone, nullptr, GWarn));
            if (!Blueprint) { Error = TEXT("Cannot create rig: ") + PackageName; return false; }
            FAssetRegistryModule::AssetCreated(Blueprint);
        }
        Blueprint->InitializeModelIfRequired(false);
        auto Controller = Blueprint->GetOrCreateController();
        Controller->RemoveNodes(Controller->GetGraph()->GetNodes(), false);
        for (const auto& Variable : Blueprint->GetMemberVariables()) Blueprint->RemoveMemberVariable(Variable.Name);
        Blueprint->Hierarchy->Reset();
        TArray<FString> Errors;
        TMap<int32, URigVMNode*> Nodes;
        if (!RestoreVariables(Blueprint, Rig, Errors) || !RestoreHierarchy(Blueprint, Rig, Errors) ||
            !CreateInstructionNodes(Blueprint, Rig, Nodes, Errors) || !RestoreDataLinks(Blueprint, Rig, Nodes, Errors) || !RestoreExecutionLinks(Blueprint, Rig, Nodes, Errors))
        { Error = PackageName + TEXT(": ") + FString::Join(Errors, TEXT("; ")); return false; }
        FCompilerResultsLog Results;
        FKismetEditorUtilities::CompileBlueprint(Blueprint, EBlueprintCompileOptions::None, &Results);
        Blueprint->RecompileVM();
        if (Results.NumErrors || Blueprint->Status == BS_Error) { Error = TEXT("Recovered rig failed compilation: ") + PackageName; return false; }
        Blueprint->MarkPackageDirty();
        FSavePackageArgs Args;
        Args.TopLevelFlags = RF_Public | RF_Standalone;
        const FString Filename = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());
        if (!UPackage::SavePackage(Package, Blueprint, *Filename, Args)) { Error = TEXT("Cannot save recovered rig: ") + PackageName; return false; }
    }
    return true;
}
}

#endif

#if ENGINE_MAJOR_VERSION == 4
#include "ControlRigGraphReconstruction.h"
#include "Dom/JsonObject.h"
namespace ControlRigGraphReconstruction {
bool BuildAssets(const TSharedPtr<FJsonObject>& Data, FString& Error) {
 const TArray<TSharedPtr<FJsonValue>>* Rigs = nullptr;
 if (!Data->TryGetArrayField(TEXT("ControlRigs"), Rigs) || Rigs->Num() == 0) return true;
 Error = TEXT("UE5 RigVM recovery data cannot be compiled by UE4.27. Re-export using UE4.27 rig metadata.");
 return false;
}
}
#endif
