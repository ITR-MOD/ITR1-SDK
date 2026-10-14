#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IInstanceID.generated.h"

UINTERFACE(Blueprintable)
class UIInstanceID : public UInterface {
    GENERATED_BODY()
};

class IIInstanceID : public IInterface {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    FString GetInstanceID();
    
};

