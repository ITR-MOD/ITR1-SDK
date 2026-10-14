#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EPRHand.h"
#include "GripSelectorsControllerInterface.generated.h"

UINTERFACE(Blueprintable)
class UGripSelectorsControllerInterface : public UInterface {
    GENERATED_BODY()
};

class IGripSelectorsControllerInterface : public IInterface {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void ShowSelector(EPRHand hand, bool Distance);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void HideSelector(EPRHand hand);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void HideAllSelectors();
    
};

