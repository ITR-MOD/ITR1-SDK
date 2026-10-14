#pragma once
#include "CoreMinimal.h"
#include "EMissionObjectiveImportance.generated.h"

UENUM(BlueprintType)
enum class EMissionObjectiveImportance : uint8 {
    Primary,
    Secondary,
    Hint,
};

