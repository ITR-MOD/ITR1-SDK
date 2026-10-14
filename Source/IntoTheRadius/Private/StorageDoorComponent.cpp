#include "StorageDoorComponent.h"

UStorageDoorComponent::UStorageDoorComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->CurrentAngle = 0.00f;
    this->AngleLimit = 120.00f;
    this->AngleSnap = 5.00f;
}

void UStorageDoorComponent::SetLock(bool lock) {
}

void UStorageDoorComponent::Open() {
}

void UStorageDoorComponent::OnTick() {
}

void UStorageDoorComponent::OnLevelStarted(FGameplayTag LevelTag) {
}

void UStorageDoorComponent::OnGameStarted() {
}

bool UStorageDoorComponent::IsOpened() const {
    return false;
}

bool UStorageDoorComponent::IsClosed() const {
    return false;
}

void UStorageDoorComponent::GetCurrentState(EStorageDoorState& CurrentState) const {
}

void UStorageDoorComponent::Close() {
}


