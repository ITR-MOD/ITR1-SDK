#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IPlayFootstepSounds.generated.h"

UINTERFACE(Blueprintable)
class UIPlayFootstepSounds : public UInterface {
    GENERATED_BODY()
};

class IIPlayFootstepSounds : public IInterface {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void StopFootstepLoop();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void StartFootstepLoop();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void PlayFootstepSound();
    
};

