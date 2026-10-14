#pragma once
#include "CoreMinimal.h"
#include "IpNetDriver.h"
#include "OculusNetDriver.generated.h"

UCLASS(Blueprintable, NonTransient)
class UOculusNetDriver : public UIpNetDriver {
    GENERATED_BODY()
public:
    UOculusNetDriver(const FObjectInitializer& ObjectInitializer);
};

