#include "HolsterComponent.h"

UHolsterComponent::UHolsterComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->Holds = NULL;
    this->WorkDistance = 0.00f;
    this->InventoryModes = 2;
}

void UHolsterComponent::CheckInteractionMode(const EInventoryMode Mode, bool& allow) const {
}




