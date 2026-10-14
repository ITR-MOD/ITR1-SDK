#pragma once
#include "CoreMinimal.h"
#include "EItemSlot.generated.h"

UENUM(BlueprintType)
enum class EItemSlot : uint8 {
    None,
    Magazine,
    Barrel,
    Stock,
    Rail_01 = 11,
    Rail_02,
    Rail_03,
    Rail_04,
    Slot_01 = 21,
    Slot_02,
    Slot_03,
    Slot_04,
};

