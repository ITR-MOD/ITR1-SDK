#pragma once
#include "CoreMinimal.h"
#include "OneParamItemDelegate.generated.h"

class UInventoryItem;

UDELEGATE(BlueprintCallable) DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOneParamItem, UInventoryItem*, InventoryItem);

