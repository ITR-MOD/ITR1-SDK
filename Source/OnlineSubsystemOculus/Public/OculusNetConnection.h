#pragma once
#include "CoreMinimal.h"
#include "IpConnection.h"
#include "OculusNetConnection.generated.h"

UCLASS(Blueprintable, NonTransient)
class UOculusNetConnection : public UIpConnection {
    GENERATED_BODY()
public:
    UOculusNetConnection();
};

