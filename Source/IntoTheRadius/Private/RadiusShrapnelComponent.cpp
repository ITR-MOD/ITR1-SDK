#include "RadiusShrapnelComponent.h"

URadiusShrapnelComponent::URadiusShrapnelComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->MaxCount = 20;
    this->TraceProfileName = TEXT("Bullet");
    this->DamageClass = NULL;
    this->ParticalScale = 1.00f;
    this->PenetrationParticalScale = 1.30f;
    this->MetalMaterial = NULL;
    this->MaxAccuracySpread = 6.00f;
    this->MinAccuracySpread = 1.00f;
    this->DistanceStep = 100.00f;
    this->MinTraceDistance = 200.00f;
    this->MaxDamageScale = 0.10f;
    this->MinDamage = 0.10f;
    this->DestroyAfterExecute = true;
}

void URadiusShrapnelComponent::Initialize(const FAmmoInfo& AmmoInfo, float Damage, float Accuracy, FVector Offset, float TraceDistance) {
}

void URadiusShrapnelComponent::Execute() {
}


