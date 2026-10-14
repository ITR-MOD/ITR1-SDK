#include "PRCommonGameData.h"

UPRCommonGameData::UPRCommonGameData() {
    this->FinishedMissionsCount = 0;
    this->MaxPlayerLevel = 0;
    this->GameTotalTicks = 0;
    this->SessionCount = 0;
    this->GuitarPlayCount = 0;
    this->bCachedHasSupporterPack = false;
}

void UPRCommonGameData::SetTrigger(const FGameplayTag Tag) {
}

void UPRCommonGameData::SaveShootingRun(const float RunTime, const int32 Scores, const int32 Hits, const int32 Misses, const int32 GrenadeKills) {
}

void UPRCommonGameData::OnMissionChange(UMissionData* MissionData) {
}

void UPRCommonGameData::OnItemTaken(UInventoryItem* Item) {
}

bool UPRCommonGameData::IsWeaponUnlocked(const FGameplayTag Tag) {
    return false;
}

void UPRCommonGameData::Initialize() {
}

int32 UPRCommonGameData::IncrementGuitarPlayCount() {
    return 0;
}

bool UPRCommonGameData::GetTrigger(const FGameplayTag Tag) {
    return false;
}

float UPRCommonGameData::GetTotalPlaytimeMinutes() {
    return 0.0f;
}

void UPRCommonGameData::GetCommonGameData(UPRCommonGameData*& Data) {
}


