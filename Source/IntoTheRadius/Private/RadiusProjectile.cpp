#include "RadiusProjectile.h"

ARadiusProjectile::ARadiusProjectile(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->AutoDisposeTime = 0.00f;
    this->DamageDisposeTime = 0.00f;
}

void ARadiusProjectile::Dispose() {
}


