#pragma once
#include "CoreMinimal.h"
#include "Sound/AudioVolume.h"
#include "Sound/AudioVolume.h"
#include "Components/BoxComponent.h"
#include "Sound/AudioVolume.h"
#include "Sound/ReverbSettings.h"
#include "RadiusAudioBoxComponent.generated.h"

class AActor;
class AAudioVolume;

UCLASS(Blueprintable, EditInlineNew, ClassGroup=Custom, meta=(BlueprintSpawnableComponent))
class INTOTHERADIUS_API URadiusAudioBoxComponent : public UBoxComponent {
    GENERATED_BODY()
public:
private:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float Priority;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    uint8 bEnabled: 1;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FReverbSettings Settings;
    
   // UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
   // FInteriorSettings AmbientZoneSettings;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FAudioVolumeSubmixSendSettings> SubmixSendSettings;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FAudioVolumeSubmixOverrideSettings> SubmixOverrideSettings;
    
protected:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    AAudioVolume* CachedVolume;
    
public:
    URadiusAudioBoxComponent(const FObjectInitializer& ObjectInitializer);

private:
    UFUNCTION(BlueprintCallable)
    void UpdateVolumeSettings();
    
public:
    UFUNCTION(BlueprintCallable)
    void SetPriority(float NewPriority);
    
    UFUNCTION(BlueprintCallable)
    void SetEnabled(bool bNewEnabled);
    
protected:
    UFUNCTION(BlueprintCallable)
    void OnOwnerDestroyed(AActor* DestoryedObject);
    
};

