#include "PRMapPoint.h"
#include "Components/SceneComponent.h"

APRMapPoint::APRMapPoint(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}


