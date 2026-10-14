#pragma once
#include "CoreMinimal.h"
#include "ELevelManagerState.generated.h"

UENUM(BlueprintType)
enum class ELevelManagerState : uint8 {
    Ready,
    Loading,
    Unloading = 3,
};

