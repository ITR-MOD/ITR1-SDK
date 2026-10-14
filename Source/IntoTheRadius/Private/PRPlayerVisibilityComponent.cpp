#include "PRPlayerVisibilityComponent.h"

UPRPlayerVisibilityComponent::UPRPlayerVisibilityComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->Player = NULL;
    this->Environment = NULL;
    this->DistanceFactorCurve = NULL;
    this->SunFactorCurve = NULL;
    this->HeadlampFactorCurve = NULL;
    this->VelocityFactorCurve = NULL;
}

void UPRPlayerVisibilityComponent::InitOnLevelStart(FGameplayTag LevelTag) {
}

void UPRPlayerVisibilityComponent::InitOnGameStart() {
}

void UPRPlayerVisibilityComponent::CleanupOnTransitionStart() {
}


