#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "PRBasicActor.h"
#include "PREnvironment.generated.h"

class ADirectionalLight;
class ALevelSequenceActor;
class APRSkyBox;
class APostProcessVolume;
class ASkyLight;

UCLASS(Abstract, Blueprintable)
class INTOTHERADIUS_API APREnvironment : public APRBasicActor {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FGameplayTag EnvironmentLevelTag;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    APRSkyBox* SkyBoxRef;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    ALevelSequenceActor* EnvLevelSequenceRef;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    ADirectionalLight* SunRef;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    ASkyLight* SkyLightRef;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    APostProcessVolume* LightScenarioPostProcessRef;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float DebugTimeScale;
    
    APREnvironment(const FObjectInitializer& ObjectInitializer);

protected:
    UFUNCTION(BlueprintCallable)
    void Startup();
    
public:
    UFUNCTION(BlueprintCallable)
    void SetEnvironmentHidden(const bool bEnvironmentHidden);
    
private:
    UFUNCTION(BlueprintCallable)
    void OnGameTimeChanged(float AddedSeconds);
    
    UFUNCTION(BlueprintCallable)
    void OnGameplayTimeDilationChanged(float Dilation);
    
public:
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FVector GetRadiusLocation() const;
    
};

