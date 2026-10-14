#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ICommandEvents.generated.h"

UINTERFACE(Blueprintable, MinimalAPI)
class UICommandEvents : public UInterface {
    GENERATED_BODY()
};

class IICommandEvents : public IInterface {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void OnWalkCommandStarted();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void OnWalkCommandFinished();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void OnIdleCommandStarted();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void OnIdleCommandFinished();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void OnAttackCommandStarted();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void OnAttackCommandFinished();
    
};

