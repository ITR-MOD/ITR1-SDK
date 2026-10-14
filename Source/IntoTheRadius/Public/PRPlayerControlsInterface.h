#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PRPlayerControlsInterface.generated.h"

UINTERFACE(Blueprintable)
class UPRPlayerControlsInterface : public UInterface {
    GENERATED_BODY()
};

class IPRPlayerControlsInterface : public IInterface {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void GrabRight(const bool bPressed);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void GrabLeft(const bool bPressed);
    
};

