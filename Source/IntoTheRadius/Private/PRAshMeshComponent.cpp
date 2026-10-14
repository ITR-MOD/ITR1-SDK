#include "PRAshMeshComponent.h"

UPRAshMeshComponent::UPRAshMeshComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->SpawnHeight = 500.00f;
    this->SpawnRadius = 4000.00f;
    this->LifeTimeMin = 12.00f;
    this->LifeTimeMax = 20.00f;
    this->ParticleSizeMin = 2.00f;
    this->ParticleSizeMax = 5.00f;
    this->ParticleSpeedMin = 30.00f;
    this->ParticleSpeedMax = 55.00f;
    this->ParticleAngularSpeedMax = 30.00f;
    this->ParticleSpinRadiusMax = 50.00f;
    this->ParticleSpinSpeedMax = 1.00f;
    this->ParticlesCount = 2048;
    this->ParticlesTileX = 3;
    this->ParticlesTileY = 3;
}

void UPRAshMeshComponent::InitParticles() {
}


