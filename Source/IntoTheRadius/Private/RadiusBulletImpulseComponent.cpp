#include "RadiusBulletImpulseComponent.h"

URadiusBulletImpulseComponent::URadiusBulletImpulseComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->_target = NULL;
    this->_speedCurve = NULL;
    this->DistanceDivider = 100.00f;
}

void URadiusBulletImpulseComponent::Suspend(bool IsSuspend) {
}

void URadiusBulletImpulseComponent::Stop() {
}

void URadiusBulletImpulseComponent::Start() {
}

void URadiusBulletImpulseComponent::Ricochet(const FVector& Location, const FVector& Force, const bool IsPenetration) {
}

void URadiusBulletImpulseComponent::Initialize(UPrimitiveComponent* TargetComponent, const float Mass, const float InitialVelocity, UCurveFloat* SpeedCurve) {
}


