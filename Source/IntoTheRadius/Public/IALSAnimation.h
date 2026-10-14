#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EGroundedEntryState.h"
#include "IALSAnimation.generated.h"

UINTERFACE(Blueprintable)
class UIALSAnimation : public UInterface {
    GENERATED_BODY()
};

class IIALSAnimation : public IInterface {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void SetOverlayOverrideState(int32 NewOverlayOverrideState);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void SetGroundedEntryState(EGroundedEntryState NewGroundedEntryState);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void ReloadWeapon();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void OnJumped();
    
};

