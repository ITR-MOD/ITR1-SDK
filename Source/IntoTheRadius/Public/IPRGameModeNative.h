#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IPRGameModeNative.generated.h"

UINTERFACE(Blueprintable)
class UIPRGameModeNative : public UInterface {
    GENERATED_BODY()
};

class IIPRGameModeNative : public IInterface {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool GetIsInTransition() const;
    
};

