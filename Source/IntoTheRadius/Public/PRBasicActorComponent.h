#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "PRBasicActorComponent.generated.h"

UCLASS(Abstract, Blueprintable, ClassGroup=Custom, meta=(BlueprintSpawnableComponent))
class INTOTHERADIUS_API UPRBasicActorComponent : public UActorComponent {
    GENERATED_BODY()
public:
    UPRBasicActorComponent(const FObjectInitializer& ObjectInitializer);

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

