#include "RadiusBulletTracerComponent.h"

URadiusBulletTracerComponent::URadiusBulletTracerComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->DebugLineDurarion = 10.00f;
    this->DebugLineThickness = 2.00f;
    this->ScaleFactor = 0.04f;
    this->VelocityTolerance = 1500.00f;
}

void URadiusBulletTracerComponent::Suspend(bool IsSuspend) {
}

void URadiusBulletTracerComponent::Stop() {
}

void URadiusBulletTracerComponent::Start() {
}

void URadiusBulletTracerComponent::Initialize(const bool ForceTracer, const float MinSpeed, const float MaxSpeed, const FGameplayTag AmmoTypeID, const bool DebugMode) {
}

void URadiusBulletTracerComponent::DelayedStart() {
}


