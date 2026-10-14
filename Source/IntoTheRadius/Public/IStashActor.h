#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "StashSpawnInfo.h"
#include "IStashActor.generated.h"

UINTERFACE(Blueprintable)
class UIStashActor : public UInterface {
    GENERATED_BODY()
};

class IIStashActor : public IInterface {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void Setup(const FStashSpawnInfo& StashSpawnInfo);
    
};

