#pragma once
#include "CoreMinimal.h"
#include "EStartupType.generated.h"

UENUM(BlueprintType)
enum class EStartupType : uint8 {
    BuildShipping,
    BuildDebug,
    EditorNormal = 3,
    EditorCustom,
};

