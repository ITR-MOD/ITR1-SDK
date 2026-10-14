#pragma once
#include "CoreMinimal.h"
#include "EPRHmdDeviceType.generated.h"

UENUM(BlueprintType)
enum class EPRHmdDeviceType : uint8 {
    Unknown,
    PIE,
    ValveIndex = 10,
    Vive = 20,
    ViveCosmos,
    Oculus = 30,
    OculusRiftS,
    WindowsMR = 40,
    Pico = 50,
};

