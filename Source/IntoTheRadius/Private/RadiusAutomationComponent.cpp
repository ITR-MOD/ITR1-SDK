#include "RadiusAutomationComponent.h"

URadiusAutomationComponent::URadiusAutomationComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->ActiveAutomation = NULL;
}

void URadiusAutomationComponent::MainMenuReadyCallback(FGameplayTag LevelTag) {
}

void URadiusAutomationComponent::GameStartedCallback() {
}


