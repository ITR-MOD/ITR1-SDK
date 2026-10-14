#include "RadiusBulletComponent.h"

URadiusBulletComponent::URadiusBulletComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->BulletTag = TEXT("Bullet");
    this->MaxImpactVelocity = 1000.00f;
    this->DefaultDensity = 1.00f;
    this->NoiseLoudness = 1.00f;
    this->NoiseRange = 800.00f;
    this->DefaultMaterial = NULL;
    this->MetalMaterial = NULL;
    this->PenetrationScale = 100.00f;
    this->RicochetForceScale = 0.50f;
}

void URadiusBulletComponent::ProcessHit(const FHitResult& HitResult, const FVector& Velocity) {
}

void URadiusBulletComponent::Initialize(const FAmmoInfo& AmmoInfo, const float MuzzleVelocity, const float BaseDamage) {
}


