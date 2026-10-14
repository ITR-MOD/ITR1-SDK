#pragma once
#include "CoreMinimal.h"
#include "EPRGameDifficultyPreset.generated.h"

UENUM(BlueprintType)
enum class EPRGameDifficultyPreset : uint8 {
    Easy,
    Normal,
    Hard,
    Custom,
};

