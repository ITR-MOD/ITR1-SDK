#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "StartupConfig.generated.h"

class UPRItemConfig;

USTRUCT(Atomic, BlueprintType)
struct INTOTHERADIUS_API FStartupConfig {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    int32 Money;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    TArray<UPRItemConfig*> Items;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FGameplayTag> Triggers;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FGameplayTag> OpenedLevels;
    
    FStartupConfig();
};

