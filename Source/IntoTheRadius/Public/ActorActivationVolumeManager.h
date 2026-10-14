#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "ActorActivationVolumeManager.generated.h"

UCLASS(Blueprintable)
class INTOTHERADIUS_API UActorActivationVolumeManager : public UObject {
    GENERATED_BODY()
public:
    UActorActivationVolumeManager();

private:
    UFUNCTION(BlueprintCallable)
    void OnTransitionStart();
    
    UFUNCTION(BlueprintCallable)
    void OnLevelPrepare(FGameplayTag LevelTag);
    
};

