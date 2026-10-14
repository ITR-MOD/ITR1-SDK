#pragma once
#include "CoreMinimal.h"
#include "ETracersShowType.generated.h"

UENUM(BlueprintType)
enum class ETracersShowType : uint8 {
    None,
    Enemies,
    Player,
    All,
};

