#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "TideTriggerConfig.generated.h"

class UPRCondition;

USTRUCT(Atomic, BlueprintType)
struct INTOTHERADIUS_API FTideTriggerConfig {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    TArray<UPRCondition*> Conditions;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FGameplayTag> TriggersToSet;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FGameplayTag> TriggersToUnset;
    
    FTideTriggerConfig();
};

