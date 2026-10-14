#include "RadiusAudioBoxComponent.h"

URadiusAudioBoxComponent::URadiusAudioBoxComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->Priority = 0.00f;
    this->bEnabled = true;
    this->CachedVolume = NULL;
}

void URadiusAudioBoxComponent::UpdateVolumeSettings() {
}

void URadiusAudioBoxComponent::SetPriority(float NewPriority) {
}

void URadiusAudioBoxComponent::SetEnabled(bool bNewEnabled) {
}

void URadiusAudioBoxComponent::OnOwnerDestroyed(AActor* DestoryedObject) {
}


