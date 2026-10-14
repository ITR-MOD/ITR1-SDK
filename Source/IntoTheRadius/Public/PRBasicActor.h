#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "RelevanceDescription.h"
#include "PRBasicActor.generated.h"

UCLASS(Abstract, Blueprintable)
class INTOTHERADIUS_API APRBasicActor : public AActor {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool UseRelevanceManagement;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FRelevanceDescription> RelevanceDescription;
    
    APRBasicActor(const FObjectInitializer& ObjectInitializer);

protected:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void OnTransitionStart();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void OnTransitionEnd();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void OnLevelStarted(const FGameplayTag LevelTag);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void OnLevelPrepare(const FGameplayTag LevelTag);
    
private:
    UFUNCTION(BlueprintCallable)
    void OnGameStartedPrivate();
    
protected:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void OnGameStarted();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void OnGamePaused(const bool bPaused);
    
    UFUNCTION(BlueprintCallable)
    void DisableRelevanceManagement();
    
};

