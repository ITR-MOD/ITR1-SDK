#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include "NpcPointQueryResult.h"
#include "RelevanceDescription.h"
#include "PRBasicCharacter.generated.h"

UCLASS(Blueprintable)
class INTOTHERADIUS_API APRBasicCharacter : public ACharacter {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool UseRelevanceManagement;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FRelevanceDescription> RelevanceDescription;
    
    APRBasicCharacter(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable)
    void SetCoverReservationFromSearchResult(const FNpcPointQueryResult& Result);
    
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
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void OnGamePaused(const bool bPaused);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void OnGameDataSaved();
    
    UFUNCTION(BlueprintCallable)
    bool GetRandomReachablePoint(FVector Origin, float Radius, FVector StartPoint, float MinDistance, FVector& Point);
    
public:
    UFUNCTION(BlueprintCallable)
    void ClearCoverReservation();
    
};

