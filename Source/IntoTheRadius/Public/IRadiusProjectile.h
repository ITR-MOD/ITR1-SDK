#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IRadiusProjectile.generated.h"

class URadiusProjectileInfo;

UINTERFACE(BlueprintType, MinimalAPI, meta=(CannotImplementInterfaceInBlueprint))
class UIRadiusProjectile : public UInterface {
    GENERATED_BODY()
};

class IIRadiusProjectile : public IInterface {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable)
    virtual void Launch() PURE_VIRTUAL(Launch,);
    
    UFUNCTION(BlueprintCallable)
    virtual void Initialize(URadiusProjectileInfo* ProjectileInfo) PURE_VIRTUAL(Initialize,);
    
};

