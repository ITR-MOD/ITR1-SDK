#include "PRMapCenter.h"
#include "Components/SceneComponent.h"

APRMapCenter::APRMapCenter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->bIsEditorOnlyActor = true;
    this->RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}


