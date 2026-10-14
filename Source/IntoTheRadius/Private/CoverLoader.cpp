#include "CoverLoader.h"
#include "CoverDebugComponent.h"

ACoverLoader::ACoverLoader(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->ShippingDebugActor = NULL;
    this->DebugComponent = CreateDefaultSubobject<UCoverDebugComponent>(TEXT("CoverDebugComponent"));
}


