#pragma once
#include "CoreMinimal.h"
#include "EMissionType.generated.h"

UENUM(BlueprintType)
enum class EMissionType : uint8 {
    Tutorial,
    Story,
    Generic,
    SpecialArtifacts,
    SpecialGenerators,
};

