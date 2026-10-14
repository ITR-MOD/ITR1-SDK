#pragma once
#include "CoreMinimal.h"
#include "EPRStatStatus.generated.h"

UENUM(BlueprintType)
enum class EPRStatStatus : uint8 {
    End,
    Low,
    Normal,
};

