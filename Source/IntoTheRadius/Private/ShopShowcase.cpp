#include "ShopShowcase.h"
#include "Components/SceneComponent.h"

AShopShowcase::AShopShowcase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    this->CurrentConfigIndex = 0;
    this->CurrentPhysicalIndex = 0;
    this->bSwitching = false;
    this->CurrentAngle = 0.00f;
    this->TargetAngle = 0.00f;
    this->ItemInShowcaseRoot = CreateDefaultSubobject<USceneComponent>(TEXT("ShowcaseItems"));
    this->ItemInShowcaseRoot->SetupAttachment(RootComponent);
}

void AShopShowcase::SwitchToIndex(int32 Index) {
}

void AShopShowcase::SetUnlockedTypes(const TArray<FGameplayTag>& GameplayTag) {
}

int32 AShopShowcase::GetSelectedIndex() const {
    return 0;
}


