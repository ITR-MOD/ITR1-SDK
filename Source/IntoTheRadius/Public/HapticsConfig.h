#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EHapticsType.h"
#include "IAsset.h"
#include "IClearable.h"
#include "IPreparable.h"
#include "HapticsConfig.generated.h"

class UDataTable;
class UHapticsConfig;

UCLASS(Blueprintable)
class INTOTHERADIUS_API UHapticsConfig : public UDataAsset, public IIAsset, public IIPreparable, public IIClearable {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    UDataTable* BHapticsTable;
    
    UHapticsConfig();

    UFUNCTION(BlueprintCallable)
    void StopHaptics();
    
    UFUNCTION(BlueprintCallable)
    void PlayRecoilHaptics(const bool bLeftHand, const bool bSecondaryGripped, const float PowerAlpha);
    
    UFUNCTION(BlueprintCallable)
    void PlayHapticsWithRotation(const EHapticsType Type, const float angleX, const float OffsetY);
    
    UFUNCTION(BlueprintCallable)
    void PlayHaptics(const EHapticsType Type);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsHapticsEnabled();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static UHapticsConfig* GetHapticsConfig();
    

    // Fix for true pure virtual functions not being implemented
};

