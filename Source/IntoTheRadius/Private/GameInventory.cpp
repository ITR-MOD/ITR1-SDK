#include "GameInventory.h"

UGameInventory::UGameInventory() {
    this->PlayerInventory = NULL;
    this->TutorialPlayerInventory = NULL;
    this->RepairInventory = NULL;
}

void UGameInventory::SetAmmoBeltContent(const FString& InstanceId, TArray<FGameplayTag> beltContent) {
}

UInventoryItem* UGameInventory::GetItem(const FString& InstanceId) {
    return NULL;
}

void UGameInventory::GetAmmoBeltContent(const FString& InstanceId, TArray<FGameplayTag>& beltContent) {
}


