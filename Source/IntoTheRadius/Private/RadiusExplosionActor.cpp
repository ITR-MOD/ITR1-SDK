#include "RadiusExplosionActor.h"

ARadiusExplosionActor::ARadiusExplosionActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->MaxIteration = 4;
    this->Strength = 1000.00f;
    this->Fallout = RIF_Constant;
    this->IsVelocityChange = false;
    this->MaxWallDistance = 500.00f;
    this->TraceProfileName = TEXT("Bullet");
    this->DamageMinRange = 200.00f;
    this->MaxDamageScale = 0.30f;
    this->HitMinDistance = 300.00f;
    this->DamageClass = NULL;
    this->ExplosionRadius = 1500.00f;
    this->WallRangeMin = 1.00f;
}


FExplosionInfo ARadiusExplosionActor::GetExplosionInfo() const {
    return FExplosionInfo{};
}


