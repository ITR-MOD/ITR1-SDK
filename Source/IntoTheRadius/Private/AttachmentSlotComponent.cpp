#include "AttachmentSlotComponent.h"
#include "Components/BoxComponent.h"

UAttachmentSlotComponent::UAttachmentSlotComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->CanCharacterStepUpOn = ECB_No;
    this->Slot = EItemSlot::None;
    this->MyStaticMesh = NULL;
    this->BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Bounds"));
}

void UAttachmentSlotComponent::TryDetachItem_Implementation(AActor* Attachment, bool& bSuccess) {
}

void UAttachmentSlotComponent::TryAttachItem_Implementation(AActor* Attachment, bool& bSuccess) {
}

void UAttachmentSlotComponent::SetSlotEnabled(const bool bEnable) {
}

bool UAttachmentSlotComponent::IsSlotEnabled() const {
    return false;
}

bool UAttachmentSlotComponent::IsSlotEmpty() const {
    return false;
}

void UAttachmentSlotComponent::IsParentHeld(bool& IsHeld) const {
}

void UAttachmentSlotComponent::IsAccepts(const FGameplayTag Type, bool& bCanAttach) const {
}

UInventoryItem* UAttachmentSlotComponent::GetParentItem() const {
    return NULL;
}


