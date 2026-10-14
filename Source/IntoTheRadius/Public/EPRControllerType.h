#pragma once
#include "CoreMinimal.h"
#include "EPRControllerType.generated.h"

UENUM(BlueprintType)
enum class EPRControllerType : uint8 {
    Unknown,
    Index = 10,
    Vive = 20,
    ViveCosmos,
    Rift = 30,
    RiftS,
    NativeRift = 40,
    NativeRiftS,
    WMR = 50,
    PicoNeo3 = 60,
    Pico4,
};

