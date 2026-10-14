#include "HapticsConfig.h"

UHapticsConfig::UHapticsConfig() {
    this->BHapticsTable = NULL;
}

void UHapticsConfig::StopHaptics() {
}

void UHapticsConfig::PlayRecoilHaptics(const bool bLeftHand, const bool bSecondaryGripped, const float PowerAlpha) {
}

void UHapticsConfig::PlayHapticsWithRotation(const EHapticsType Type, const float angleX, const float OffsetY) {
}

void UHapticsConfig::PlayHaptics(const EHapticsType Type) {
}

bool UHapticsConfig::IsHapticsEnabled() {
    return false;
}

UHapticsConfig* UHapticsConfig::GetHapticsConfig() {
    return NULL;
}


