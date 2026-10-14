#include "RadiusItemActor.h"

ARadiusItemActor::ARadiusItemActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->ProxyModel = NULL;
    this->bCanAddToLevelInventory = true;
    this->ProxyComp = NULL;
}


