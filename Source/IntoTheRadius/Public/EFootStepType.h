#pragma once
#include "CoreMinimal.h"
#include "EFootStepType.generated.h"

UENUM(BlueprintType)
enum class EFootStepType : uint8 {
    Step,
    WalkOrRun,
    Jump,
    Land,
};

