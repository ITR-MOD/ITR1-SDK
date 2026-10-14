#pragma once
#include "CoreMinimal.h"
#include "EHapticsCurve.generated.h"

UENUM(BlueprintType)
enum class EHapticsCurve : uint8 {
    Interact,
    FirePistol,
    Grab,
};

