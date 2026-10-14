#pragma once
#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "GameplayTagContainer.h"
#include "PRBasicSceneComponent.generated.h"

UCLASS(Abstract, Blueprintable, ClassGroup=Custom, meta=(BlueprintSpawnableComponent))
class INTOTHERADIUS_API UPRBasicSceneComponent : public USceneComponent {
    GENERATED_BODY()
public:
    UPRBasicSceneComponent(const FObjectInitializer& ObjectInitializer);

protected:
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnLevelStarted(FGameplayTag LevelTag);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnLevelPrepare(FGameplayTag LevelTag);
    
private:
    UFUNCTION(BlueprintCallable)
    void OnGameStartedPrivate();
    
protected:
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnGameStarted();
    
};

