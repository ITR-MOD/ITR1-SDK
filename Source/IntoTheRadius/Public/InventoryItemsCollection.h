#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "IClearable.h"
#include "InventoryItemsCollection.generated.h"

class UInventoryItem;

UCLASS(Blueprintable)
class INTOTHERADIUS_API UInventoryItemsCollection : public UObject, public IIClearable {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<UInventoryItem*> Items;
    
    UInventoryItemsCollection();


    // Fix for true pure virtual functions not being implemented
};

