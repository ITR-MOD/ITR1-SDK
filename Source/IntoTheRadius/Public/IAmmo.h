#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AmmoInfo.h"
#include "IAmmo.generated.h"

UINTERFACE(Blueprintable, MinimalAPI)
class UIAmmo : public UInterface {
    GENERATED_BODY()
};

class IIAmmo : public IInterface {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    FAmmoInfo GetAmmoInfo() const;
    
};

