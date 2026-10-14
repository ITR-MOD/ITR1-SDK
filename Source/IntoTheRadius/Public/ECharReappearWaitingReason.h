#pragma once
#include "CoreMinimal.h"
#include "ECharReappearWaitingReason.generated.h"

UENUM(BlueprintType)
enum class ECharReappearWaitingReason : uint8 {
    None,
    WaitForReappearDelay,
    WaitForCoverSearch,
    WaitToLeaveView,
};

