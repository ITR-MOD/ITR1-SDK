#include "RadiusNiagaraEmitter.h"
#include "NiagaraComponent.h"
#include "PoolDisposeComponent.h"

ARadiusNiagaraEmitter::ARadiusNiagaraEmitter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->PoolDisposeComponent = CreateDefaultSubobject<UPoolDisposeComponent>(TEXT("PoolDisposeComponent"));
    this->ParticleSystemComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ParticleSystemComponent"));
    this->DisposeTime = 10.00f;
}


