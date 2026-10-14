#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ShopShowcaseSideConfig.generated.h"

USTRUCT(BlueprintType)
struct INTOTHERADIUS_API FShopShowcaseSideConfig {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FText Name;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FGameplayTag> Items;
    
    FShopShowcaseSideConfig();
};

