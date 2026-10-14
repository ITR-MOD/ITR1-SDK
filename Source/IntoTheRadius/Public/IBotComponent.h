#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IBotComponent.generated.h"

UINTERFACE(Blueprintable, MinimalAPI)
class UIBotComponent : public UInterface {
    GENERATED_BODY()
};

class IIBotComponent : public IInterface {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void OwnerInitialized();
    
};

