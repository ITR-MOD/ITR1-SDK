#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AmmoContainerData.generated.h"

USTRUCT(Atomic, BlueprintType)
struct INTOTHERADIUS_API FAmmoContainerData {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FGameplayTag> Ammo;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FGameplayTag ChamberAmmo;
    
    FAmmoContainerData();
};

