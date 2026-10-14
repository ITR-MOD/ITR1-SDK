#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EPRControllerType.h"
#include "IPRPlayerController.generated.h"

UINTERFACE(Blueprintable, MinimalAPI)
class UIPRPlayerController : public UInterface {
    GENERATED_BODY()
};

class IIPRPlayerController : public IInterface {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    bool IsAllInputBlocked();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void InitControlScheme();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    EPRControllerType GetControllerType();
    
};

