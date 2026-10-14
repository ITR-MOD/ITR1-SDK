#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MissionTriggersConfig.generated.h"

USTRUCT(Atomic, BlueprintType)
struct INTOTHERADIUS_API FMissionTriggersConfig {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FGameplayTag> TriggersToSet;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FGameplayTag> TriggersToUnset;
    
    FMissionTriggersConfig();
};

