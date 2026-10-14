#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ExplosionInfo.h"
#include "IRadiusExplosionActor.generated.h"

UINTERFACE(Blueprintable)
class UIRadiusExplosionActor : public UInterface {
    GENERATED_BODY()
};

class IIRadiusExplosionActor : public IInterface {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void Launch();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void Initialize(const FExplosionInfo& ExplosionInfo);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void Dispose();
    
};

