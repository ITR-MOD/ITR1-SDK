#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ActorVolumeActivationInterface.generated.h"

UINTERFACE(Blueprintable, MinimalAPI)
class UActorVolumeActivationInterface : public UInterface {
    GENERATED_BODY()
};

class IActorVolumeActivationInterface : public IInterface {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void SetVolumeActivationState(bool NewState);
    
};

