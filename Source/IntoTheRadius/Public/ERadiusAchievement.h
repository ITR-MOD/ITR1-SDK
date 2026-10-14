#pragma once
#include "CoreMinimal.h"
#include "ERadiusAchievement.generated.h"

UENUM(BlueprintType)
enum class ERadiusAchievement : uint8 {
    None,
    FirstWeapon,
    FirstDeath,
    CompleteShootingRange = 10,
    CompleteIronman,
    ClimbCrane = 20,
    KillFromDistance,
    KillWithBarrel,
    HitByStomper,
    CollectArtifacts = 30,
    CollectLoreCassettes,
};

