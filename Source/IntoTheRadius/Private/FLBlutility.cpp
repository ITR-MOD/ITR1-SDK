#include "FLBlutility.h"

UFLBlutility::UFLBlutility() {
}

bool UFLBlutility::SetWeaponInfosToTable(UDataTable* DataTable, TArray<FWeaponInfo> infos) {
    return false;
}

bool UFLBlutility::SetPoseInfosToTable(UDataTable* DataTable, TArray<FPRHandPoseInfosPair> Poses) {
    return false;
}

bool UFLBlutility::SetNPCInfosToTable(UDataTable* DataTable, TArray<FPRNPCInfo> infos) {
    return false;
}

bool UFLBlutility::SetMobility(AActor* Actor, const TEnumAsByte<EComponentMobility::Type> Mobility) {
    return false;
}

bool UFLBlutility::SetItemInfosToTable(UDataTable* DataTable, TArray<FPRItemInfo> infos) {
    return false;
}

bool UFLBlutility::SetAnomalyInfosToTable(UDataTable* DataTable, TArray<FPRAnomalyInfo> infos) {
    return false;
}

bool UFLBlutility::SetAmmoInfosToTable(UDataTable* DataTable, TArray<FAmmoInfo> infos) {
    return false;
}

bool UFLBlutility::SetAmmoContainerInfosToTable(UDataTable* DataTable, TArray<FPRAmmoContainerInfo> infos) {
    return false;
}

bool UFLBlutility::SetAllowCullDistanceVolume(UStaticMeshComponent* Component, bool bValue) {
    return false;
}

void UFLBlutility::SaveDataTable(UDataTable* DataTable) {
}

void UFLBlutility::ReloadConfigAssets() {
}

void UFLBlutility::LoadItemInfos(TArray<FPRItemInfo>& Array, bool updateTable) {
}

bool UFLBlutility::CompareHandPoses(FPRHandPoseInfo poseA, FPRHandPoseInfo poseB) {
    return false;
}

void UFLBlutility::ClearDataTable(UDataTable* DataTable) {
}

bool UFLBlutility::AddWeaponInfoToTable(UDataTable* DataTable, FWeaponInfo Info) {
    return false;
}

bool UFLBlutility::AddPoseInfoToTable(UDataTable* DataTable, FPRHandPoseInfosPair Pose) {
    return false;
}

TMap<FGameplayTag, FTagToTransformsList> UFLBlutility::AddPointData(TMap<FGameplayTag, FTagToTransformsList> In, FGameplayTag LevelTag, FGameplayTag pointTag, FTransform absoluteTransform, TMap<FGameplayTag, FTransform> levelCenters, bool Message, bool duplicateCheck) {
    return TMap<FGameplayTag, FTagToTransformsList>();
}

bool UFLBlutility::AddNPCInfoToTable(UDataTable* DataTable, FPRNPCInfo Info) {
    return false;
}

bool UFLBlutility::AddItemInfoToTable(UDataTable* DataTable, FPRItemInfo Info) {
    return false;
}

bool UFLBlutility::AddAnomalyInfoToTable(UDataTable* DataTable, FPRAnomalyInfo Info) {
    return false;
}

bool UFLBlutility::AddAmmoInfoToTable(UDataTable* DataTable, FAmmoInfo Info) {
    return false;
}

bool UFLBlutility::AddAmmoContainerInfoToTable(UDataTable* DataTable, FPRAmmoContainerInfo Info) {
    return false;
}


