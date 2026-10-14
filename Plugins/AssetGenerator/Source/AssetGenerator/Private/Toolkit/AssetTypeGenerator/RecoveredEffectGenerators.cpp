#include "Toolkit/AssetTypeGenerator/RecoveredEffectGenerators.h"
#include "UE427AssetPath.h"
#include "GraphReconstruction/ControlRigGraphReconstruction.h"
#if __has_include("ControlRigBlueprint.h")
#include "ControlRigBlueprint.h"
#else
#include "ControlRigBlueprintLegacy.h"
#endif
#include "NiagaraSystem.h"
#include "Factories/Factory.h"
#include "Dom/JsonObject.h"
#include "UObject/MetaData.h"
#include "Animation/PoseAsset.h"
#include "Animation/Skeleton.h"
#include "UObject/UnrealType.h"

FTopLevelAssetPath URecoveredControlRigGenerator::GetAssetClass() { return FTopLevelAssetPath(UControlRigBlueprint::StaticClass()); }
void URecoveredControlRigGenerator::CreateAssetPackage() {
    FString Error;
    if (!ControlRigGraphReconstruction::BuildAssets(GetAssetData(), Error)) { FailGeneration(Error); return; }
    UPackage* Package = FindPackage(nullptr, *GetPackageName().ToString());
    auto Asset = Package ? FindObject<UControlRigBlueprint>(Package, *GetAssetName().ToString()) : nullptr;
    if (!Asset) { FailGeneration(TEXT("Recovered Control Rig asset was not created.")); return; }
    SetPackageAndAsset(Package, Asset);
    MarkAssetChanged();
}
void URecoveredControlRigGenerator::OnExistingPackageLoaded() {
    FString Error;
    if (!ControlRigGraphReconstruction::BuildAssets(GetAssetData(), Error)) { FailGeneration(Error); return; }
    MarkAssetChanged();
}
FTopLevelAssetPath URecoveredNiagaraGenerator::GetAssetClass() { return FTopLevelAssetPath(UNiagaraSystem::StaticClass()); }
void URecoveredNiagaraGenerator::CreateAssetPackage() {
    UPackage* Package = CreatePackage(*GetPackageName().ToString());
    UClass* FactoryClass = LoadClass<UFactory>(nullptr, TEXT("/Script/NiagaraEditor.NiagaraSystemFactoryNew"));
    if (!FactoryClass) { FailGeneration(TEXT("Niagara system factory unavailable.")); return; }
    UFactory* Factory = NewObject<UFactory>(GetTransientPackage(), FactoryClass);
    auto Asset = Cast<UNiagaraSystem>(Factory->FactoryCreateNew(UNiagaraSystem::StaticClass(), Package, GetAssetName(), RF_Public | RF_Standalone, nullptr, GWarn));
    if (!Asset) { FailGeneration(TEXT("Could not create Niagara placeholder.")); return; }
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 6
    Package->GetMetaData().SetValue(Asset, TEXT("CASPlaceholder"), TEXT("CookedNiagaraNotRecovered"));
#else
    Package->GetMetaData()->SetValue(Asset, TEXT("CASPlaceholder"), TEXT("CookedNiagaraNotRecovered"));
#endif
    SetPackageAndAsset(Package, Asset);
    MarkAssetChanged();
    UE_LOG(LogAssetGenerator, Warning, TEXT("Created empty Niagara placeholder %s; it produces no visual effect."), *Asset->GetPathName());
}
void URecoveredNiagaraGenerator::OnExistingPackageLoaded() {}

UClass* URecoveredPoseAssetGenerator::GetAssetObjectClass() const { return UPoseAsset::StaticClass(); }
FTopLevelAssetPath URecoveredPoseAssetGenerator::GetAssetClass() { return FTopLevelAssetPath(UPoseAsset::StaticClass()); }
void URecoveredPoseAssetGenerator::PopulateSimpleAssetWithData(UObject* Asset) {
    Super::PopulateSimpleAssetWithData(Asset);
    auto PoseAsset = CastChecked<UPoseAsset>(Asset);
    auto Skeleton = PoseAsset->GetSkeleton();
    if (!Skeleton) { FailGeneration(TEXT("Pose asset skeleton is missing.")); return; }
    auto Property = FindFProperty<FStructProperty>(PoseAsset->GetClass(), TEXT("PoseContainer"));
    auto Container = Property->ContainerPtrToValuePtr<FPoseDataContainer>(PoseAsset);
    const auto Struct = FPoseDataContainer::StaticStruct();
    auto Tracks = FindFProperty<FArrayProperty>(Struct, TEXT("Tracks"))->ContainerPtrToValuePtr<TArray<FName>>(Container);
    auto Poses = FindFProperty<FArrayProperty>(Struct, TEXT("Poses"))->ContainerPtrToValuePtr<TArray<FPoseData>>(Container);
    auto Indices = FindFProperty<FArrayProperty>(Struct, TEXT("TrackBoneIndices"))->ContainerPtrToValuePtr<TArray<int32>>(Container);
    const auto& Reference = Skeleton->GetReferenceSkeleton();
    TArray<FTransform> Base;
    Indices->Reset();
    for (const FName Track : *Tracks) {
        const int32 Index = Reference.FindBoneIndex(Track);
        if (Index == INDEX_NONE) { FailGeneration(TEXT("Pose track is missing from skeleton: ") + Track.ToString()); return; }
        Indices->Add(Index);
        Base.Add(Reference.GetRefBonePose()[Index]);
    }
    TArray<float> BaseCurves;
    const int32 BaseIndex = PoseAsset->GetBasePoseIndex();
    if (PoseAsset->IsValidAdditive() && Poses->IsValidIndex(BaseIndex)) {
        const auto& BasePose = (*Poses)[BaseIndex];
        BaseCurves = BasePose.CurveData;
        for (int32 Track = 0; Track < Tracks->Num(); ++Track) {
            if (const int32* BufferIndex = BasePose.TrackToBufferIndex.Find(Track)) {
                if (!BasePose.LocalSpacePose.IsValidIndex(*BufferIndex)) { FailGeneration(TEXT("Invalid base pose track index.")); return; }
                Base[Track] = BasePose.LocalSpacePose[*BufferIndex];
            }
        }
    }
    for (int32 PoseIndex = 0; PoseIndex < Poses->Num(); ++PoseIndex) {
        auto& Pose = (*Poses)[PoseIndex];
        Pose.SourceLocalSpacePose = Base;
        Pose.SourceCurveData = Pose.CurveData;
        if (PoseAsset->IsValidAdditive() && PoseIndex != BaseIndex) {
            for (int32 Curve = 0; Curve < Pose.SourceCurveData.Num(); ++Curve)
                if (BaseCurves.IsValidIndex(Curve)) Pose.SourceCurveData[Curve] += BaseCurves[Curve];
        }
        for (int32 Track = 0; Track < Tracks->Num(); ++Track) {
            if (const int32* BufferIndex = Pose.TrackToBufferIndex.Find(Track)) {
                if (!Pose.LocalSpacePose.IsValidIndex(*BufferIndex)) { FailGeneration(TEXT("Invalid cooked pose track index.")); return; }
                const auto& Transform = Pose.LocalSpacePose[*BufferIndex];
                if (PoseAsset->IsValidAdditive() && PoseIndex != BaseIndex) Pose.SourceLocalSpacePose[Track].AccumulateWithAdditiveScale(Transform, ScalarRegister(1.f));
                else Pose.SourceLocalSpacePose[Track] = Transform;
            }
        }
    }
    PoseAsset->ConvertSpace(PoseAsset->IsValidAdditive(), PoseAsset->GetBasePoseIndex());
    MarkAssetChanged();
}
