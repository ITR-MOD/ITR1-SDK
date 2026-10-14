#pragma once
#include "CoreMinimal.h"
#include "EDigitsCompareMethod.generated.h"

UENUM(BlueprintType)
enum class EDigitsCompareMethod : uint8 {
    EqualTo,
    NotEqualTo,
    GreaterThanOrEqualTo,
    LessThanOrEqualTo,
    GreaterThan,
    LessThan,
};

