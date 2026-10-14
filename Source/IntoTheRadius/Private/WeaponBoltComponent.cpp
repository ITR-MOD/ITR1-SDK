#include "WeaponBoltComponent.h"

UWeaponBoltComponent::UWeaponBoltComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->AngleLimit = -90.00f;
    this->AutoCloseRotateOnRelease = true;
    this->SnapPercent = 0.20f;
    this->SlideLimit = -10.00f;
    this->StickToOpen = 1.00f;
    this->RotationCoeff = 10.00f;
    this->SlideCoeff = 2.00f;
}

void UWeaponBoltComponent::OnTick() {
}

void UWeaponBoltComponent::OnLevelStarted(FGameplayTag LevelTag) {
}

void UWeaponBoltComponent::OnGameStarted() {
}

bool UWeaponBoltComponent::IsOpened() const {
    return false;
}

bool UWeaponBoltComponent::IsClosed() const {
    return false;
}

void UWeaponBoltComponent::GetCurrentState(EWeaponBoltState& CurrentState) const {
}

void UWeaponBoltComponent::Drop() {
}


