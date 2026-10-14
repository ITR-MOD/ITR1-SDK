#pragma once
#include "CoreMinimal.h"
#include "ESpeedCoefficient.generated.h"

UENUM(BlueprintType)
enum class ESpeedCoefficient : uint8 {
    Default,
    Anomaly,
    Tide,
    TutorialZone,
    WaterPatch,
};

