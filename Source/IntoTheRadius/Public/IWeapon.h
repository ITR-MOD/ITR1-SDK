#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WeaponInfo.h"
#include "IWeapon.generated.h"

UINTERFACE(Blueprintable, MinimalAPI)
class UIWeapon : public UInterface {
    GENERATED_BODY()
};

class IIWeapon : public IInterface {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool IsSmoothingScriptOn();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    FWeaponInfo GetWeaponInfo() const;
    
};

