#pragma once
#include "CoreMinimal.h"
#include "ERadiusCommonStatisticsType.generated.h"

UENUM(BlueprintType)
enum class ERadiusCommonStatisticsType : uint8 {
    TimePlayed,
    FinishedMissions,
    CollectedArtifacts,
    CollectedCassettes,
    CollectedWeaponPrimary,
    CollectedWeaponSecondary,
};

