#include "MissionAbstractObjectiveData.h"

UMissionAbstractObjectiveData::UMissionAbstractObjectiveData() {
    this->Importance = EMissionObjectiveImportance::Primary;
    this->RewardMoney = 0;
    this->RewardPlayerLevel = 0;
    this->Finished = false;
}

bool UMissionAbstractObjectiveData::HaveDelivery() {
    return false;
}

int32 UMissionAbstractObjectiveData::GetRewardMoney() const {
    return 0;
}

FText UMissionAbstractObjectiveData::GetObjectiveShortName() {
    return FText::GetEmpty();
}

FText UMissionAbstractObjectiveData::GetObjectiveLongName() {
    return FText::GetEmpty();
}

TArray<FGameplayTag> UMissionAbstractObjectiveData::GetMissionPoints() {
    return TArray<FGameplayTag>();
}

FString UMissionAbstractObjectiveData::GetDeliveryCountString() {
    return TEXT("");
}

int32 UMissionAbstractObjectiveData::GetDeliveryCountObjectives() {
    return 0;
}

FString UMissionAbstractObjectiveData::GetCountString() {
    return TEXT("");
}

int32 UMissionAbstractObjectiveData::GetCountObjectives() {
    return 0;
}

void UMissionAbstractObjectiveData::CollectDeliveryTags(TArray<FGameplayTag>& DeliveryTags) {
}

bool UMissionAbstractObjectiveData::CheckObjectiveCompletion() {
    return false;
}


