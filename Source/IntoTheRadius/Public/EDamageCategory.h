#pragma once
#include "CoreMinimal.h"
#include "EDamageCategory.generated.h"

UENUM(BlueprintType)
enum class EDamageCategory : uint8 {
    None,
    Physical,
    Anomaly,
    Environmental,
    Technical,
    Gas,
};

