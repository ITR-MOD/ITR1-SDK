#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IHealthStatus.generated.h"

UINTERFACE(Blueprintable)
class UIHealthStatus : public UInterface {
    GENERATED_BODY()
};

class IIHealthStatus : public IInterface {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool IsAlive();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void GetHealthPercent(float& HealthPercent);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void GetHealth(float& Health);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void ChangeHealth(float HealthDelta);
    
};

