#pragma once
#include "CoreMinimal.h"
#include "EMissionState.generated.h"

UENUM(BlueprintType)
enum class EMissionState : uint8 {
    Ready,
    Briefing,
    Debriefing,
    Executing = 10,
    Successful = 20,
    Failed,
    Rejected,
    Missed,
};

