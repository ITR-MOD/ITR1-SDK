#include "PRClusterPointArtifact.h"
#include "Components/BillboardComponent.h"

UPRClusterPointArtifact::UPRClusterPointArtifact(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Root"));
}


