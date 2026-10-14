#pragma once
#include "CoreMinimal.h"
#include "EBodyArmorSetupTypes.generated.h"

UENUM(BlueprintType)
enum class EBodyArmorSetupTypes : uint8 {
    AddNewMesh,
    SwitchBodyMesh,
    SwitchBodyMaterials,
};

