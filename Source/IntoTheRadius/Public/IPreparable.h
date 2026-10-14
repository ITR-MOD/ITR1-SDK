#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IPreparable.generated.h"

UINTERFACE(Blueprintable)
class UIPreparable : public UInterface {
    GENERATED_BODY()
};

class IIPreparable : public IInterface {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void Prepare();
    
};

