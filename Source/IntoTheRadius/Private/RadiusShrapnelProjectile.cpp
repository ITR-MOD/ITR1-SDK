#include "RadiusShrapnelProjectile.h"
#include "RadiusShrapnelComponent.h"

ARadiusShrapnelProjectile::ARadiusShrapnelProjectile(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->ShrapnelComponent = CreateDefaultSubobject<URadiusShrapnelComponent>(TEXT("ShrapnelComponent"));
}


