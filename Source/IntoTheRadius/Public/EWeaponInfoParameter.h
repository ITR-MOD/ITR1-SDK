#pragma once
#include "CoreMinimal.h"
#include "EWeaponInfoParameter.generated.h"

UENUM(BlueprintType)
enum class EWeaponInfoParameter : uint8 {
    Damage,
    Recoil,
    Accuracy,
    FireRate,
    Reliability,
};

