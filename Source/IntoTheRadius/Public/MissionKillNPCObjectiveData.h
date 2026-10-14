#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MissionAbstractObjectiveData.h"
#include "TagToTagsList.h"
#include "MissionKillNPCObjectiveData.generated.h"

UCLASS(Blueprintable)
class INTOTHERADIUS_API UMissionKillNPCObjectiveData : public UMissionAbstractObjectiveData {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FTagToTagsList> PointsToNPCs;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float SpawnNPCsRadius;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FGameplayTag> ClearedPoints;
    
    UMissionKillNPCObjectiveData();

    UFUNCTION(BlueprintCallable, BlueprintPure)
    FString PointSpawnerID(FGameplayTag pointTag);
    
    UFUNCTION(BlueprintCallable)
    void MarkPointCleared(FGameplayTag pointTag);
    
};

