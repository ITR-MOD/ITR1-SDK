#pragma once
#include "CoreMinimal.h"
#include "ECharDetailLevel.generated.h"

UENUM(BlueprintType)
enum class ECharDetailLevel : uint8 {
    Normal,
    PendingDisable,
    Disabled,
};

