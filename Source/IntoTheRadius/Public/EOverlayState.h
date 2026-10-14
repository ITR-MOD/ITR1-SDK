#pragma once
#include "CoreMinimal.h"
#include "EOverlayState.generated.h"

UENUM(BlueprintType)
enum class EOverlayState : uint8 {
    Default,
    Masculine,
    Feminine,
    Injured,
    HandsTied,
    Rifle,
    Pistol1H,
    Pistol2H,
    Bow,
    Torch,
    Binoculars,
    Box,
    Barrel,
};

