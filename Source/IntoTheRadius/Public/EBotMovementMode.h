#pragma once
#include "CoreMinimal.h"
#include "EBotMovementMode.generated.h"

UENUM(BlueprintType)
enum class EBotMovementMode : uint8 {
    WalkIdle,
    CrouchIdle,
    Sprint,
    RunIdle,
    WalkAiming,
    CrouchAiming,
    RunAiming,
};

