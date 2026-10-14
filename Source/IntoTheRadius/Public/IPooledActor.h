#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/NoExportTypes.h"
#include "IPooledActor.generated.h"

UINTERFACE(Blueprintable)
class UIPooledActor : public UInterface {
    GENERATED_BODY()
};

class IIPooledActor : public IInterface {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void PushToPoolActor();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void PopFromPoolActor(const FTransform Transform);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool IsExcludedFromPool();
    
};

