#pragma once
#include "CoreMinimal.h"
#include "EGraphicsSettingsType.generated.h"

UENUM(BlueprintType)
enum class EGraphicsSettingsType : uint8 {
    ViewDistance,
    ShadowQuality,
    FoliageDensity,
    Effects,
    Textures,
    Geometry,
};

