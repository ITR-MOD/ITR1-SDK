#pragma once
#include "CoreMinimal.h"
#include "EPRPlatformType.generated.h"

UENUM(BlueprintType)
enum class EPRPlatformType : uint8 {
    Unknown,
    Oculus,
    Steam,
    Quest,
    Pico,
};

