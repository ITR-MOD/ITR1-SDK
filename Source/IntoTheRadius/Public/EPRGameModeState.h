#pragma once
#include "CoreMinimal.h"
#include "EPRGameModeState.generated.h"

UENUM(BlueprintType)
enum class EPRGameModeState : uint8 {
    Init,
    Ready,
    TransitionUnloading = 10,
    TransitionLoading,
    TransitionWaitingForPSO,
    TransitionPreparing,
    TransitionStarting,
};

