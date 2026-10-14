#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ReadyMarkInterface.generated.h"

UINTERFACE(Blueprintable)
class UReadyMarkInterface : public UInterface {
    GENERATED_BODY()
};

class IReadyMarkInterface : public IInterface {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool ReadyTrigger() const;
    
};

