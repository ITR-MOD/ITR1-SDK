#include "MissionsManager.h"

UMissionsManager::UMissionsManager() {
}

void UMissionsManager::StartMissionFromIdLatent(bool& bSuccess, FGameplayTag GameplayTag, UObject* WorldContextObject, FLatentActionInfo LatentInfo) {
}

void UMissionsManager::StartMission(const FString& MissionInstanceId, bool& bSuccess, UMissionData*& MissionData) {
}

void UMissionsManager::SetSelectedMissionComplete(const FString& MissionInstanceId) {
}

void UMissionsManager::ResetTutorialMissions() {
}

void UMissionsManager::OnPlayerLevelChanged(const int32 PlayerLevel) {
}

void UMissionsManager::IsMissionSelected(const FString& MissionInstanceId, bool& bMissionSelected) {
}

bool UMissionsManager::IsMaxExecutingReached() {
    return false;
}

bool UMissionsManager::GetStoryMissionUnlockInfo(int32& FinishedAfterPrevStory, int32& RequiredAfterPrevStory) {
    return false;
}

void UMissionsManager::GetSelectedMissionsInState(const EMissionState State, TArray<UMissionData*>& SelectedMissions) {
}

TArray<UMissionData*> UMissionsManager::GetSelectedMissionsInProgress() {
    return TArray<UMissionData*>();
}

void UMissionsManager::GetSelectedMissions(TArray<UMissionData*>& SelectedMissions) {
}

UMissionsManager* UMissionsManager::GetMissionsManager() {
    return NULL;
}

int32 UMissionsManager::GetMissionsFinishedAfter(const FGameplayTag ConfigID, const bool bOnlySuccessful) {
    return 0;
}

void UMissionsManager::GetMission(const FString& MissionInstanceId, UMissionData*& MissionData) {
}

void UMissionsManager::GetFinishedMissions(TArray<UMissionData*>& FinishedMissions) {
}

bool UMissionsManager::GetAvailableStoryMission(UMissionData*& StoryMission) {
    return false;
}

void UMissionsManager::GetActiveMissions(TMap<FGameplayTag, UMissionData*>& ActiveMissions) {
}

void UMissionsManager::GetActiveMissionOfType(const FGameplayTag& Category, UMissionData*& MissionData) {
}

bool UMissionsManager::GenerateNewMissions(bool bTide) {
    return false;
}

bool UMissionsManager::FinishMission(const FString& MissionInstanceId) {
    return false;
}

void UMissionsManager::CheckActive() {
}

void UMissionsManager::AbortSelectedMission(const FString& MissionInstanceId) {
}


