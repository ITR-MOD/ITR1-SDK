#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "PRTriggerConfig.generated.h"

class UPRCondition;

USTRUCT(Atomic, BlueprintType)
struct INTOTHERADIUS_API FPRTriggerConfig {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FGameplayTag Trigger;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    TArray<UPRCondition*> Conditions;
    
    FPRTriggerConfig();
};

