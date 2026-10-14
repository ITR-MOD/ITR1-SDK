#include "RadiusImpactActor.h"
#include "PoolDisposeComponent.h"

ARadiusImpactActor::ARadiusImpactActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->PoolDisposeComponent = CreateDefaultSubobject<UPoolDisposeComponent>(TEXT("PoolDisposeComponent"));
}



