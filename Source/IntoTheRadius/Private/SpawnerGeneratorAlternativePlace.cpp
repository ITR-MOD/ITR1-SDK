#include "SpawnerGeneratorAlternativePlace.h"
#include "Components/BillboardComponent.h"

ASpawnerGeneratorAlternativePlace::ASpawnerGeneratorAlternativePlace(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->RootComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("Root"));
}


