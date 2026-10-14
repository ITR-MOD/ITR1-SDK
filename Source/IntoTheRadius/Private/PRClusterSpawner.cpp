#include "PRClusterSpawner.h"
#include "Components/BillboardComponent.h"

APRClusterSpawner::APRClusterSpawner(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->RootComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("Root"));
    this->ProjectAnomalies = true;
}

void APRClusterSpawner::OnCheckTick() {
}


