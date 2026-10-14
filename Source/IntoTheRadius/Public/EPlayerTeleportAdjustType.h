#pragma once
#include "CoreMinimal.h"
#include "EPlayerTeleportAdjustType.generated.h"

UENUM(BlueprintType)
enum class EPlayerTeleportAdjustType : uint8 {
    None,
    Location,
    LocationAndRotation,
};

