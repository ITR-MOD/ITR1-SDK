#include "RadiusPooledActor.h"
#include "PoolDisposeComponent.h"

ARadiusPooledActor::ARadiusPooledActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->PoolDisposeComponent = CreateDefaultSubobject<UPoolDisposeComponent>(TEXT("PoolDisposeComponent"));
    this->bExcludedFromPool = false;
}


