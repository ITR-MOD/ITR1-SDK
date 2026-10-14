#include "PRClusterPattern.h"
#include "Components/SceneComponent.h"

APRClusterPattern::APRClusterPattern(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    this->Root = (USceneComponent*)RootComponent;
}


