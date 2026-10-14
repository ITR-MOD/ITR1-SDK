#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "EPRControllerType.h"
#include "EPRHmdDeviceType.h"
#include "EStartupType.h"
#include "PRGIReferences.h"
#include "RadiusGameDifficulty.h"
#include "RadiusGameInstanceInterface.h"
#include "PRGameInstance.generated.h"

class UAnalyticsManager;
class USentrySettings;
class UTextureRenderTarget2D;

UCLASS(Abstract, Blueprintable, NonTransient)
class INTOTHERADIUS_API UPRGameInstance : public UPRGIReferences, public IRadiusGameInstanceInterface {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    EStartupType StartupType;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FString StartupLevelName;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<FGameplayTag, UTextureRenderTarget2D*> MapsRTs;
    
    UPRGameInstance();

protected:
    UFUNCTION(BlueprintCallable)
    bool SetShutdown();
    
    UFUNCTION(BlueprintCallable)
    void RestartGame(const int32 Slot, const bool skipTutorial, const bool Ironman, const FRadiusGameDifficulty Difficulty);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void ReceiveOnGameStart();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnShutdown();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnBeforeLoad();
    
public:
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsEAPlayer();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsBetaPlayer() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FString HmdToString(EPRHmdDeviceType HMD);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool HideVersionRevision() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool HasSupporterPack() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void HasSaveOnStartup(bool& hasSave);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool GetPurchaseDate(FDateTime& PurchaseDate, FString& PurchaseDateString);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetPlayerID(FString& ID);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetPlayerCountry(FString& countryCode);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetHMDDeviceType(EPRHmdDeviceType& HMD);
    
    UFUNCTION(BlueprintCallable)
    void GetDeviceTypeString(FString& HMD, FString& Controller);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetControllerType(EPRControllerType& Controller, bool& bIsDefault);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, BlueprintPure)
    UAnalyticsManager* GetAnalyticsManager();
    
protected:
    UFUNCTION(BlueprintCallable)
    void FixTutorialSaveLevel();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void ExitGameBP(const bool bMenu, const bool bSave);
    
private:
    UFUNCTION(BlueprintCallable)
    void DoStart();
    
public:
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FString ControllerToString(EPRControllerType Controller);
    
private:
//    UFUNCTION(BlueprintCallable)
 //   void ConfigureSentrySubsystem_Delegate(USentrySettings* Settings) const;
    

    // Fix for true pure virtual functions not being implemented
};

