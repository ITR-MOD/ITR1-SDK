#pragma once
#include "CoreMinimal.h"
#include "EWeaponBoltState.generated.h"

UENUM(BlueprintType)
enum class EWeaponBoltState : uint8 {
    Closed,
    Rotating,
    HalfWay,
    Sliding,
    Opened,
};

