#pragma once
#include "CoreMinimal.h"
#include "EPRHandPoseGeometryType.generated.h"

UENUM(BlueprintType)
enum class EPRHandPoseGeometryType : uint8 {
    Point,
    Spline,
    Count,
};

