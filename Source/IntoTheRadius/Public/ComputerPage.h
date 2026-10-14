#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ComputerPage.generated.h"

class AActor;

UINTERFACE(Blueprintable)
class UComputerPage : public UInterface {
    GENERATED_BODY()
};

class IComputerPage : public IInterface {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void ShowPage();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void PageTick();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void InitializeByActor(AActor* Actor);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void HidePage();
    
};

