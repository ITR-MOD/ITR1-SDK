#pragma once
#include "CoreMinimal.h"
#include "EPRHandPoseType.generated.h"

UENUM(BlueprintType)
enum class EPRHandPoseType : uint8 {
    Primary,
    PrimaryTriggered,
    PrimarySafety,
    Secondary = 10,
    Throw = 20,
    Slide = 30,
};

