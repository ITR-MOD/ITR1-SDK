#include "PRPlayerGripSystemComponent.h"
#include "DistanceGripManager.h"

UPRPlayerGripSystemComponent::UPRPlayerGripSystemComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->OwningPlayer = NULL;
    this->DistanceGripManager = CreateDefaultSubobject<UDistanceGripManager>(TEXT("Distance Grip Manager"));
}

void UPRPlayerGripSystemComponent::UnregisterForDistanceGrip(UInventoryItem* Item) {
}

void UPRPlayerGripSystemComponent::ToggleDebugDrawDistanceGrip() {
}

void UPRPlayerGripSystemComponent::RegisterForDistanceGrip(UInventoryItem* Item) {
}

void UPRPlayerGripSystemComponent::PerformOverlapGrip(const bool isLeftHand, const TArray<AActor*>& otherHandGrippedActors, const USphereComponent* GripSphere, const FTransform& HandWorldTransform, const FVector& handRootLocation, const UGripMotionControllerComponent* MotionController, UPrimitiveComponent*& CompToGrip, UPrimitiveComponent*& triggerToGrip, FTransform& triggerTransform) {
}

void UPRPlayerGripSystemComponent::PerformDistanceGrip(const USphereComponent* GripSphere, UPrimitiveComponent*& CompToGrip, FVector& CompHitLoc, FVector& DistanceGripPoint) {
}

void UPRPlayerGripSystemComponent::IsDebugDrawDistanceGrip(bool& bDebugDrawing) const {
}


