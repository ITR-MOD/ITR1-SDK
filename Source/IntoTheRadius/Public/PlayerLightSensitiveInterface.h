#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/NoExportTypes.h"
#include "PlayerLightSensitiveInterface.generated.h"

class USceneComponent;

UINTERFACE(Blueprintable)
class UPlayerLightSensitiveInterface : public UInterface {
    GENERATED_BODY()
};

class IPlayerLightSensitiveInterface : public IInterface {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void LightOn(FVector Location, USceneComponent* Component);
    
};

