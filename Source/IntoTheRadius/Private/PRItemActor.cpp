#include "PRItemActor.h"
#include "Components/BoxComponent.h"
#include "PRItemDurabilityComponent.h"

APRItemActor::APRItemActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->RootComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("ItemRootPrimitive"));
    this->_inventoryItem = NULL;
    this->ItemRootPrimitive = (UBoxComponent*)RootComponent;
    this->DurabilityComponent = CreateDefaultSubobject<UPRItemDurabilityComponent>(TEXT("DurabilityComponent"));
}


