#pragma once
#include "CoreMinimal.h"
#include "EMapMarker.generated.h"

UENUM(BlueprintType)
enum class EMapMarker : uint8 {
    None,
    MissionStart = 10,
    MissionPrimary,
    MissionSecondary,
    MissionHint,
    LastDeath = 20,
    LastCamp,
    Custom_1 = 30,
    PlayerPosition = 40,
    GatePosition,
    GateShortcut,
    GateHub,
};

