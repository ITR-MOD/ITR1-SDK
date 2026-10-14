#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ILootSpawner.generated.h"

UINTERFACE(Blueprintable)
class UILootSpawner : public UInterface {
    GENERATED_BODY()
};

class IILootSpawner : public IInterface {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void SpawnLoot();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void GenerateLoot();
    
};

