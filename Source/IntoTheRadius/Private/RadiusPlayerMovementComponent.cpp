#include "RadiusPlayerMovementComponent.h"

URadiusPlayerMovementComponent::URadiusPlayerMovementComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->ArcControllerClass = NULL;
    this->ArcController = NULL;
    this->CharacterOwner = NULL;
    this->OwnerCapsule = NULL;
    this->MovementComponent = NULL;
    this->StatsComponent = NULL;
}

void URadiusPlayerMovementComponent::StopMovement() {
}

void URadiusPlayerMovementComponent::ProcessMovementTeleport(const float DeltaSeconds) {
}

void URadiusPlayerMovementComponent::ProcessMovementNormal(const float DeltaSeconds) {
}

void URadiusPlayerMovementComponent::ProcessMovement(const float DeltaSeconds) {
}

TArray<FTeleportPointInfo> URadiusPlayerMovementComponent::PreparePointsToTeleport(const FVector& From, const FVector& To) {
    return TArray<FTeleportPointInfo>();
}

bool URadiusPlayerMovementComponent::IsTeleporting() const {
    return false;
}


