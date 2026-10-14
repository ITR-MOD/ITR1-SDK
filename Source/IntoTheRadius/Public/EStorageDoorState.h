#pragma once
#include "CoreMinimal.h"
#include "EStorageDoorState.generated.h"

UENUM(BlueprintType)
enum class EStorageDoorState : uint8 {
    Closed,
    Opened,
    Gripping = 10,
    Lerping,
};

