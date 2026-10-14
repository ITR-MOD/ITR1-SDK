#pragma once
#include "CoreMinimal.h"
#include "OneParamItemsArrayDelegate.generated.h"

class UInventoryItem;

UDELEGATE(BlueprintCallable) DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOneParamItemsArray, const TArray<UInventoryItem*>&, inventoryItems);

