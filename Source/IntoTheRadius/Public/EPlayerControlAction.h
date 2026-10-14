#pragma once
#include "CoreMinimal.h"
#include "EPlayerControlAction.generated.h"

UENUM(BlueprintType)
enum class EPlayerControlAction : uint8 {
    InteractLeft,
    InteractRight,
    GrabLeft,
    GrabRight,
    Move,
    Turn,
    Sprint,
    Menu,
    FireSelectorLeft = 10,
    FireSelectorRight,
    EjectMagLeft,
    EjectMagRight,
};

