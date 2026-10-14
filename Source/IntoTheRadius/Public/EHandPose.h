#pragma once
#include "CoreMinimal.h"
#include "EHandPose.generated.h"

UENUM(BlueprintType)
enum class EHandPose : uint8 {
    Default,
    Open,
    CanGrab = 3,
    GrabFull = 5,
    SolveFingers = 10,
    ConfiguredPose = 20,
    FingerPoint = 22,
    FingerPressed,
    ConfiguredPoseSecondary = 30,
    ConfiguredTriggerPose,
};

