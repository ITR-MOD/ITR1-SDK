#pragma once
#include "CoreMinimal.h"
#include "EPRSubtitleType.generated.h"
//
UENUM(BlueprintType)
enum class EPRSubtitleType : uint8 {
    Record,
    Player,
    Katya,
    Presence,
    Unknown = 50,
};
//
