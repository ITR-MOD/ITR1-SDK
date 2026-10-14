#pragma once
#include "CoreMinimal.h"
#include "EAwarenessLevel.generated.h"

UENUM(BlueprintType)
enum class EAwarenessLevel : uint8 {
    None,
    Low,
    High,
};

