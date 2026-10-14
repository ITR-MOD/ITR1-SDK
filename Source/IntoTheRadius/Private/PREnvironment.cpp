#include "PREnvironment.h"

APREnvironment::APREnvironment(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->SkyBoxRef = NULL;
    this->EnvLevelSequenceRef = NULL;
    this->SunRef = NULL;
    this->SkyLightRef = NULL;
    this->LightScenarioPostProcessRef = NULL;
    this->DebugTimeScale = 1.00f;
}

void APREnvironment::Startup() {
}

void APREnvironment::SetEnvironmentHidden(const bool bEnvironmentHidden) {
}

void APREnvironment::OnGameTimeChanged(float AddedSeconds) {
}

void APREnvironment::OnGameplayTimeDilationChanged(float Dilation) {
}

FVector APREnvironment::GetRadiusLocation() const {
    return FVector{};
}


