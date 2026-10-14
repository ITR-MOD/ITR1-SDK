#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/NoExportTypes.h"
#include "UObject/NoExportTypes.h"
#include "UObject/NoExportTypes.h"
#include "EGraphicsSettingsType.h"
#include "EPRControllerType.h"
#include "ESettingsValue.h"
#include "IClearable.h"
#include "IPreparable.h"
#include "OnSeatedModeChangedDelegate.h"
#include "OnVignetteChangedDelegate.h"
#include "SettingsManager.generated.h"

class UINIManager;
class USettingsManager;

UCLASS(Blueprintable)
class INTOTHERADIUS_API USettingsManager : public UObject, public IIPreparable, public IIClearable {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOnSeatedModeChanged OnSeatedModeChanged;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOnVignetteChanged OnVignetteChanged;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float MaxArmLength;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float MinArmLength;
    
private:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    UINIManager* INIManager;
    
public:
    USettingsManager();

    UFUNCTION(BlueprintCallable, BlueprintPure)
    int32 TurnType() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void SetVRSSIndex(const int32 Value) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void SetVolumeSounds(const float Value) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void SetVolumeMusic(const float Value) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void SetVolumeMaster(const float Value) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void SetVolumeAmbient(const float Value) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void SetVirtualStock(const bool bValue) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void SetVignetteShown(const bool bValue) const;
    
    UFUNCTION(BlueprintCallable)
    void SetVignetteRunShown(const bool bValue);
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void SetVestShift(const float Value) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void SetUpscaleSharpen(const float Value) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void SetUpscaleMethod(const int32 Value) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void SetUpdateNoticeShown() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void SetTurnType(const int32 Value) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void SetTurnSpeed(float Value) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FString SettingsValueToString(const ESettingsValue Value);
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void SetTeleportMovement(bool bValue) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void SetSSREnabled(const bool bValue) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void SetShowSubtitles(const bool bValue) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void SetShowBody(const bool bValue) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void SetShowAiDebug(const bool bValue) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void SetSeatedMode(const bool bValue) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void SetPlayerHeight(const float Value) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void SetNeckHeight(const float Value) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void SetMovementDirectionType(const int32 Value) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void SetMirrorHolsters(const bool bValue) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void SetLeftHanded(const bool bValue) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void SetLanguageSetting(const FString& Value) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void SetImperialUnits(const bool bValue) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void SetHoldGrip(bool bValue) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void SetHmdDeviceType(const FString& Value) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void SetHighlightDroppedItems(const bool bValue) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void SetGraphicsSetting(const EGraphicsSettingsType Type, const ESettingsValue Value) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void SetGraphicsQualityIndex(const int32 Value) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void SetGraphicsPresetIndex(const int32 Value) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void SetGrabWithTrigger(bool bValue) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void SetGPUQualityIndex(const int32 Value) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void SetFSRMode(const int32 Value) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void SetFirstLaunch(const bool bValue) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void SetFFRIndex(const int32 Value) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void SetControllerType(const FString& Value) const;
    
    UFUNCTION(BlueprintCallable)
    void SetControllerShift(const EPRControllerType& Controller, const FVector Value);
    
    UFUNCTION(BlueprintCallable)
    void SetControllerRotation(const EPRControllerType& Controller, const FRotator Value);
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void SetBrightnessAdjustment(const float Value) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void SetBHapticsEnabled(const bool bValue) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void SetArmLength(const float Value) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void SetAntialiasingIndex(const int32 Value) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void SetAimSmoothingIntensity(const float Value) const;
    
    UFUNCTION(BlueprintCallable)
    void RestoreDefaultControllerRotation(const EPRControllerType& Controller);
    
    UFUNCTION(BlueprintCallable)
    void ResetGraphicsToDefault(bool bIsMobile);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    int32 MovementDirectionType() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsVirtualStock() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsVignetteShown() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsVignetteRunShown() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsTeleportMovement() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsSSREnabled() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsShowUpdateNotice() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsShowSubtitles() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsShowBody() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsShowAiDebug() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsSeatedMode() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsMirrorHolsters() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsLeftHanded() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsImperialUnits() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsHoldGrip() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsGrabWithTrigger() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsFirstLaunch() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsBHapticsEnabled() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsAchievementsDisabled() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FString GraphicsSettingsTypeToString(const EGraphicsSettingsType Value);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    int32 GetVRSSIndex() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    float GetVolumeSounds() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    float GetVolumeMusic() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    float GetVolumeMaster() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    float GetVolumeAmbient() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    float GetVestShift() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    float GetUpscaleSharpen() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    int32 GetUpscaleMethod() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    float GetTurnSpeed() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FVector GetTeleportArcRotationForSecondaryGrip() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static USettingsManager* GetSettingsManager();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    TArray<int32> GetScreenPercentages() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    int32 GetScreenPercentage();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    float GetPlayerHeight() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    float GetPixelDensity();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    float GetNeckHeight() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FString GetLanguageSetting() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FString GetHMDDeviceType() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    ESettingsValue GetGraphicsSetting(const EGraphicsSettingsType Type) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    int32 GetGraphicsQualityIndex() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    int32 GetGraphicsPresetIndex() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    int32 GetGPUQualityIndex() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    int32 GetFSRMode() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    int32 GetFFRIndex() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FString GetControllerType() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetControllerTransform(const EPRControllerType& Controller, FTransform& Left, FTransform& Right) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FVector GetControllerShift(const EPRControllerType& Controller) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FRotator GetControllerRotation(const EPRControllerType& Controller) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    float GetBrightnessAdjustment() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    float GetArmLength() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    int32 GetAntialiasingIndex() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    float GetAimSmoothingIntensity() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool AreDroppedItemsHighlighted() const;
    

    // Fix for true pure virtual functions not being implemented
};

