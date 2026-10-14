#pragma once
#include "CoreMinimal.h"
#include "BaseItemSpawnInfo.h"
#include "RadiusItemActor.h"
#include "RadiusBaseItem.generated.h"

class UInventoryItem;

UCLASS(Blueprintable)
class INTOTHERADIUS_API ARadiusBaseItem : public ARadiusItemActor {
    GENERATED_BODY()
public:
    ARadiusBaseItem(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void Setup(const FBaseItemSpawnInfo& BaseItemSpawnInfo);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void ResetInventoryItemRef();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    bool IsInitInventoryItem();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    UInventoryItem* InitInventoryItem();
    
};

