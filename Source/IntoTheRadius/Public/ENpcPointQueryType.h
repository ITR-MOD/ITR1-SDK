#pragma once
#include "CoreMinimal.h"
#include "ENpcPointQueryType.generated.h"

UENUM(BlueprintType)
enum class ENpcPointQueryType : uint8 {
    None,
    Cover,
    Shooting,
    Assault,
};

