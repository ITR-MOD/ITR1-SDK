#include "RadiusEmitter.h"
#include "Particles/ParticleSystemComponent.h"
#include "PoolDisposeComponent.h"

ARadiusEmitter::ARadiusEmitter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->RootComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystemComponent"));
    this->PoolDisposeComponent = CreateDefaultSubobject<UPoolDisposeComponent>(TEXT("PoolDisposeComponent"));
    this->ParticleSystemComponent = (UParticleSystemComponent*)RootComponent;
    this->ReflectionParamaterName = TEXT("ReflectedParticles");
    this->DisposeTime = 10.00f;
}

UParticleSystemComponent* ARadiusEmitter::GetParticalSystemRef() {
    return NULL;
}


