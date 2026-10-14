#include "PRSublevelStreamingVolume.h"
#include "Components/BoxComponent.h"

APRSublevelStreamingVolume::APRSublevelStreamingVolume(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->RootComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxVolume"));
    this->Triggered = false;
    this->BoxVolume = (UBoxComponent*)RootComponent;
}

void APRSublevelStreamingVolume::UpdateVolume() {
}

void APRSublevelStreamingVolume::Reset() {
}

void APRSublevelStreamingVolume::OnTransitionStart_Implementation() {
}

void APRSublevelStreamingVolume::OnTransitionEnd_Implementation() {
}

void APRSublevelStreamingVolume::OnLevelStarted_Implementation(FGameplayTag LevelTag) {
}

void APRSublevelStreamingVolume::OnLevelPrepare_Implementation(FGameplayTag LevelTag) {
}

void APRSublevelStreamingVolume::OnGameStartedPrivate() {
}

void APRSublevelStreamingVolume::OnGameStarted_Implementation() {
}

void APRSublevelStreamingVolume::LoadSublevels(bool evented) {
}

bool APRSublevelStreamingVolume::IsLoaded() {
    return false;
}

bool APRSublevelStreamingVolume::CheckSphere(FVector Center, float Radius, float Coefficient) {
    return false;
}

bool APRSublevelStreamingVolume::CheckLocation(FVector Location, float Coefficient) {
    return false;
}


