#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MissionBaseConfig.h"
#include "MissionTriggersConfig.h"
#include "TagToTagsList.h"
#include "MissionNormalConfig.generated.h"

class UMissionBaseHintObjectiveConfig;
class UMissionBaseObjectiveConfig;

UCLASS(Blueprintable)
class INTOTHERADIUS_API UMissionNormalConfig : public UMissionBaseConfig {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    int32 CooldownMissions;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    TArray<UMissionBaseObjectiveConfig*> PrimaryObjectives;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    TArray<UMissionBaseObjectiveConfig*> SecondaryObjectives;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    TArray<UMissionBaseHintObjectiveConfig*> HintObjectives;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<FGameplayTag, FGameplayTag> AdditionalCorpsePointsToLoot;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FTagToTagsList> AdditionalPointsToNPCs;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FMissionTriggersConfig SuccessTriggers;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FMissionTriggersConfig FailTriggers;
    
    UMissionNormalConfig();

    UFUNCTION(BlueprintCallable)
    bool CheckObjectives();
    
};

