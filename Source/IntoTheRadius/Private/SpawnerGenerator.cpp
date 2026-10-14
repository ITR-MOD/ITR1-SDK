#include "SpawnerGenerator.h"

ASpawnerGenerator::ASpawnerGenerator(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->CooldownSeconds = 30.00f;
    this->HitPoints = 10.00f;
    this->ForceSpecial = false;
}

void ASpawnerGenerator::StopGenerator() {
}

bool ASpawnerGenerator::Special() const {
    return false;
}

void ASpawnerGenerator::SaveGeneratorData() {
}


void ASpawnerGenerator::CallSpawner(AMonsterSpawner* Spawner, bool& bSuccess, AActor*& NPCActor) {
}


