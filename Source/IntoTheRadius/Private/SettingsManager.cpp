#include "SettingsManager.h"

USettingsManager::USettingsManager() {
    this->MaxArmLength = 80.00f;
    this->MinArmLength = 40.00f;
    this->INIManager = NULL;
}

int32 USettingsManager::TurnType() const {
    return 0;
}

void USettingsManager::SetVRSSIndex(const int32 Value) const {
}

void USettingsManager::SetVolumeSounds(const float Value) const {
}

void USettingsManager::SetVolumeMusic(const float Value) const {
}

void USettingsManager::SetVolumeMaster(const float Value) const {
}

void USettingsManager::SetVolumeAmbient(const float Value) const {
}

void USettingsManager::SetVirtualStock(const bool bValue) const {
}

void USettingsManager::SetVignetteShown(const bool bValue) const {
}

void USettingsManager::SetVignetteRunShown(const bool bValue) {
}

void USettingsManager::SetVestShift(const float Value) const {
}

void USettingsManager::SetUpscaleSharpen(const float Value) const {
}

void USettingsManager::SetUpscaleMethod(const int32 Value) const {
}

void USettingsManager::SetUpdateNoticeShown() const {
}

void USettingsManager::SetTurnType(const int32 Value) const {
}

void USettingsManager::SetTurnSpeed(float Value) const {
}

FString USettingsManager::SettingsValueToString(const ESettingsValue Value) {
    return TEXT("");
}

void USettingsManager::SetTeleportMovement(bool bValue) const {
}

void USettingsManager::SetSSREnabled(const bool bValue) const {
}

void USettingsManager::SetShowSubtitles(const bool bValue) const {
}

void USettingsManager::SetShowBody(const bool bValue) const {
}

void USettingsManager::SetShowAiDebug(const bool bValue) const {
}

void USettingsManager::SetSeatedMode(const bool bValue) const {
}

void USettingsManager::SetPlayerHeight(const float Value) const {
}

void USettingsManager::SetNeckHeight(const float Value) const {
}

void USettingsManager::SetMovementDirectionType(const int32 Value) const {
}

void USettingsManager::SetMirrorHolsters(const bool bValue) const {
}

void USettingsManager::SetLeftHanded(const bool bValue) const {
}

void USettingsManager::SetLanguageSetting(const FString& Value) const {
}

void USettingsManager::SetImperialUnits(const bool bValue) const {
}

void USettingsManager::SetHoldGrip(bool bValue) const {
}

void USettingsManager::SetHmdDeviceType(const FString& Value) const {
}

void USettingsManager::SetHighlightDroppedItems(const bool bValue) const {
}

void USettingsManager::SetGraphicsSetting(const EGraphicsSettingsType Type, const ESettingsValue Value) const {
}

void USettingsManager::SetGraphicsQualityIndex(const int32 Value) const {
}

void USettingsManager::SetGraphicsPresetIndex(const int32 Value) const {
}

void USettingsManager::SetGrabWithTrigger(bool bValue) const {
}

void USettingsManager::SetGPUQualityIndex(const int32 Value) const {
}

void USettingsManager::SetFSRMode(const int32 Value) const {
}

void USettingsManager::SetFirstLaunch(const bool bValue) const {
}

void USettingsManager::SetFFRIndex(const int32 Value) const {
}

void USettingsManager::SetControllerType(const FString& Value) const {
}

void USettingsManager::SetControllerShift(const EPRControllerType& Controller, const FVector Value) {
}

void USettingsManager::SetControllerRotation(const EPRControllerType& Controller, const FRotator Value) {
}

void USettingsManager::SetBrightnessAdjustment(const float Value) const {
}

void USettingsManager::SetBHapticsEnabled(const bool bValue) const {
}

void USettingsManager::SetArmLength(const float Value) const {
}

void USettingsManager::SetAntialiasingIndex(const int32 Value) const {
}

void USettingsManager::SetAimSmoothingIntensity(const float Value) const {
}

void USettingsManager::RestoreDefaultControllerRotation(const EPRControllerType& Controller) {
}

void USettingsManager::ResetGraphicsToDefault(bool bIsMobile) {
}

int32 USettingsManager::MovementDirectionType() const {
    return 0;
}

bool USettingsManager::IsVirtualStock() const {
    return false;
}

bool USettingsManager::IsVignetteShown() const {
    return false;
}

bool USettingsManager::IsVignetteRunShown() const {
    return false;
}

bool USettingsManager::IsTeleportMovement() const {
    return false;
}

bool USettingsManager::IsSSREnabled() const {
    return false;
}

bool USettingsManager::IsShowUpdateNotice() const {
    return false;
}

bool USettingsManager::IsShowSubtitles() const {
    return false;
}

bool USettingsManager::IsShowBody() const {
    return false;
}

bool USettingsManager::IsShowAiDebug() const {
    return false;
}

bool USettingsManager::IsSeatedMode() const {
    return false;
}

bool USettingsManager::IsMirrorHolsters() const {
    return false;
}

bool USettingsManager::IsLeftHanded() const {
    return false;
}

bool USettingsManager::IsImperialUnits() const {
    return false;
}

bool USettingsManager::IsHoldGrip() const {
    return false;
}

bool USettingsManager::IsGrabWithTrigger() const {
    return false;
}

bool USettingsManager::IsFirstLaunch() const {
    return false;
}

bool USettingsManager::IsBHapticsEnabled() const {
    return false;
}

bool USettingsManager::IsAchievementsDisabled() const {
    return false;
}

FString USettingsManager::GraphicsSettingsTypeToString(const EGraphicsSettingsType Value) {
    return TEXT("");
}

int32 USettingsManager::GetVRSSIndex() const {
    return 0;
}

float USettingsManager::GetVolumeSounds() const {
    return 0.0f;
}

float USettingsManager::GetVolumeMusic() const {
    return 0.0f;
}

float USettingsManager::GetVolumeMaster() const {
    return 0.0f;
}

float USettingsManager::GetVolumeAmbient() const {
    return 0.0f;
}

float USettingsManager::GetVestShift() const {
    return 0.0f;
}

float USettingsManager::GetUpscaleSharpen() const {
    return 0.0f;
}

int32 USettingsManager::GetUpscaleMethod() const {
    return 0;
}

float USettingsManager::GetTurnSpeed() const {
    return 0.0f;
}

FVector USettingsManager::GetTeleportArcRotationForSecondaryGrip() const {
    return FVector{};
}

USettingsManager* USettingsManager::GetSettingsManager() {
    return NULL;
}

TArray<int32> USettingsManager::GetScreenPercentages() const {
    return TArray<int32>();
}

int32 USettingsManager::GetScreenPercentage() {
    return 0;
}

float USettingsManager::GetPlayerHeight() const {
    return 0.0f;
}

float USettingsManager::GetPixelDensity() {
    return 0.0f;
}

float USettingsManager::GetNeckHeight() const {
    return 0.0f;
}

FString USettingsManager::GetLanguageSetting() const {
    return TEXT("");
}

FString USettingsManager::GetHMDDeviceType() const {
    return TEXT("");
}

ESettingsValue USettingsManager::GetGraphicsSetting(const EGraphicsSettingsType Type) const {
    return ESettingsValue::Low;
}

int32 USettingsManager::GetGraphicsQualityIndex() const {
    return 0;
}

int32 USettingsManager::GetGraphicsPresetIndex() const {
    return 0;
}

int32 USettingsManager::GetGPUQualityIndex() const {
    return 0;
}

int32 USettingsManager::GetFSRMode() const {
    return 0;
}

int32 USettingsManager::GetFFRIndex() const {
    return 0;
}

FString USettingsManager::GetControllerType() const {
    return TEXT("");
}

void USettingsManager::GetControllerTransform(const EPRControllerType& Controller, FTransform& Left, FTransform& Right) const {
}

FVector USettingsManager::GetControllerShift(const EPRControllerType& Controller) const {
    return FVector{};
}

FRotator USettingsManager::GetControllerRotation(const EPRControllerType& Controller) const {
    return FRotator{};
}

float USettingsManager::GetBrightnessAdjustment() const {
    return 0.0f;
}

float USettingsManager::GetArmLength() const {
    return 0.0f;
}

int32 USettingsManager::GetAntialiasingIndex() const {
    return 0;
}

float USettingsManager::GetAimSmoothingIntensity() const {
    return 0.0f;
}

bool USettingsManager::AreDroppedItemsHighlighted() const {
    return false;
}


