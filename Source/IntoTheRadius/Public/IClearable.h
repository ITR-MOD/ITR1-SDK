#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IClearable.generated.h"

UINTERFACE(Blueprintable)
class UIClearable : public UInterface {
    GENERATED_BODY()
};

class IIClearable : public IInterface {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void Clear();
    
};

