#include "PRLootSpawner.h"

APRLootSpawner::APRLootSpawner(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->InstanceIdPrefix = TEXT("LootSpawner-");
    this->TryProject = true;
    this->ExcludeNonFit = false;
    this->LootPoints = 3;
}

void APRLootSpawner::OnCheckDistanceTick() {
}


