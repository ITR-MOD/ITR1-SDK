#pragma once
#include "CoreMinimal.h"
#include "EWeaponFireMode.generated.h"

UENUM(BlueprintType)
enum class EWeaponFireMode : uint8 {
    Safety,
    Single,
    FullAuto,
    Burst3,
};

