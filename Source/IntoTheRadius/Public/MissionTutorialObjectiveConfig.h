#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MissionBaseObjectiveConfig.h"
#include "MissionTutorialObjectiveConfig.generated.h"

UCLASS(Blueprintable, EditInlineNew)
class INTOTHERADIUS_API UMissionTutorialObjectiveConfig : public UMissionBaseObjectiveConfig {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FGameplayTag SequencerID;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FGameplayTag MapPoint;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FGameplayTag> ConfigTags;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool ApplyReward;
    
    UMissionTutorialObjectiveConfig();

};

