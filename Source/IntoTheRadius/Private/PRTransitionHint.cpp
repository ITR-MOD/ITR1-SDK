#include "PRTransitionHint.h"
#include "Components/SceneComponent.h"
#include "Components/WidgetComponent.h"

APRTransitionHint::APRTransitionHint(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    this->Root = (USceneComponent*)RootComponent;
    this->HintWidgetParentComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Pivot_Hint"));
    this->HintWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("W_Hint"));
    this->HintWidgetParentComponent->SetupAttachment(RootComponent);
    this->HintWidgetComponent->SetupAttachment(HintWidgetParentComponent);
}

void APRTransitionHint::ShowTransitionHint() {
}

void APRTransitionHint::HideTransitionHint() {
}


