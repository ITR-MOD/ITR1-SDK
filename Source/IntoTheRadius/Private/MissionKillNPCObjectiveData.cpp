#include "MissionKillNPCObjectiveData.h"

UMissionKillNPCObjectiveData::UMissionKillNPCObjectiveData() {
    this->SpawnNPCsRadius = 1000.00f;
}

FString UMissionKillNPCObjectiveData::PointSpawnerID(FGameplayTag pointTag) {
    return TEXT("");
}

void UMissionKillNPCObjectiveData::MarkPointCleared(FGameplayTag pointTag) {
}


