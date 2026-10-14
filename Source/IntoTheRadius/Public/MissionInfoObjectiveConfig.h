#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MissionBaseHintObjectiveConfig.h"
#include "MissionInfoObjectiveConfig.generated.h"

UCLASS(Blueprintable, EditInlineNew)
class INTOTHERADIUS_API UMissionInfoObjectiveConfig : public UMissionBaseHintObjectiveConfig {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FGameplayTag> Points;
    
    UMissionInfoObjectiveConfig();

};

