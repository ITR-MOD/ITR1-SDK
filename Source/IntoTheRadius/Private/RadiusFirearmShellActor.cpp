#include "RadiusFirearmShellActor.h"
#include "PoolDisposeComponent.h"

ARadiusFirearmShellActor::ARadiusFirearmShellActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->PoolDisposeComponent = CreateDefaultSubobject<UPoolDisposeComponent>(TEXT("PoolDisposeComponent"));
}



