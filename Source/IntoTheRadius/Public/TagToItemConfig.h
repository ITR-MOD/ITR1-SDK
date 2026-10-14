#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "TagToItemConfig.generated.h"

class UPRItemConfig;

USTRUCT(Atomic, BlueprintType)
struct INTOTHERADIUS_API FTagToItemConfig {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FGameplayTag Tag;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    UPRItemConfig* Config;
    
    FTagToItemConfig();
};

