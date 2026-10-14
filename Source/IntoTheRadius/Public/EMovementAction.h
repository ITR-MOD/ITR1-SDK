#pragma once
#include "CoreMinimal.h"
#include "EMovementAction.generated.h"

UENUM(BlueprintType)
enum class EMovementAction : uint8 {
    None,
    LowMantle,
    HighMantle,
    Rolling,
    GettingUp,
};

