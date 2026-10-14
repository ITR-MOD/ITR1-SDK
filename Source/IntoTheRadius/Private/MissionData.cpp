#include "MissionData.h"

UMissionData::UMissionData() {
    this->Abortable = true;
    this->State = EMissionState::Ready;
    this->TimeCreated = 0.00f;
    this->TimeStarted = 0.00f;
    this->TimeFinished = 0.00f;
    this->TutorialStep = 0;
}

void UMissionData::SetTutorStep(FGameplayTag SequencerID, int32 step) {
}

bool UMissionData::HaveDelivery() {
    return false;
}

void UMissionData::GetTotalReward(const bool FinishedOnly, int32& MoneyReward, int32& PlayerLevel) {
}

float UMissionData::GetTimeTillAbortAvailable() {
    return 0.0f;
}

FString UMissionData::GetStashUID(UMissionAbstractObjectiveData* Objective, FGameplayTag Tag) const {
    return TEXT("");
}

TArray<FGameplayTag> UMissionData::GetMissionPoints() {
    return TArray<FGameplayTag>();
}

FText UMissionData::GetMissionName() const {
    return FText::GetEmpty();
}

FText UMissionData::GetMissionDescription() const {
    return FText::GetEmpty();
}

FGameplayTag UMissionData::GetMissionCategoryID() const {
    return FGameplayTag{};
}

FGameplayTag UMissionData::GetMainObjectivePoint() const {
    return FGameplayTag{};
}

FGameplayTag UMissionData::GetMainObjectiveLevel() const {
    return FGameplayTag{};
}

FString UMissionData::GetItemUID(UMissionAbstractObjectiveData* Objective, int32 Index) const {
    return TEXT("");
}

void UMissionData::GetDuration(bool& bHaveDuration, FString& FinishDateString) {
}

bool UMissionData::FinishObjective(UMissionAbstractObjectiveData* Objective, bool bIgnoreTutorialDelivery) {
    return false;
}

bool UMissionData::FinishAllObjectives(bool bIgnoreTutorialDelivery) {
    return false;
}

bool UMissionData::CheckPrimaryObjectivesCompleted() {
    return false;
}

void UMissionData::CheckDelivery(TArray<AActor*> ItemActors, bool bFinishObjectives, bool& bSuccess, TArray<AActor*>& ExcessiveActors) {
}

void UMissionData::CheckCompletion(bool& bCompleted) {
}


