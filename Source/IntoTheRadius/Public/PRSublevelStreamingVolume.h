#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "ILevelPreparator.h"
#include "PRSublevelStreamingVolume.generated.h"

class UBoxComponent;

UCLASS(Blueprintable)
class INTOTHERADIUS_API APRSublevelStreamingVolume : public AActor, public IILevelPreparator {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool Triggered;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    UBoxComponent* BoxVolume;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FVector VolumeExtent;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FName> Sublevels;
    
    APRSublevelStreamingVolume(const FObjectInitializer& ObjectInitializer);

protected:
    UFUNCTION(BlueprintCallable)
    void UpdateVolume();
    
private:
    UFUNCTION(BlueprintCallable)
    void Reset();
    
protected:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void OnTransitionStart();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void OnTransitionEnd();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void OnLevelStarted(FGameplayTag LevelTag);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void OnLevelPrepare(FGameplayTag LevelTag);
    
private:
    UFUNCTION(BlueprintCallable)
    void OnGameStartedPrivate();
    
protected:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void OnGameStarted();
    
public:
    UFUNCTION(BlueprintCallable)
    void LoadSublevels(bool evented);
    
    UFUNCTION(BlueprintCallable)
    bool IsLoaded();
    
    UFUNCTION(BlueprintCallable)
    bool CheckSphere(FVector Center, float Radius, float Coefficient);
    
    UFUNCTION(BlueprintCallable)
    bool CheckLocation(FVector Location, float Coefficient);
    

    // Fix for true pure virtual functions not being implemented
};

