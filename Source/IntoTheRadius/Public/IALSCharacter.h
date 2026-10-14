#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/NoExportTypes.h"
#include "UObject/NoExportTypes.h"
#include "Engine/EngineTypes.h"
#include "EGait.h"
#include "EMovementAction.h"
#include "EMovementState.h"
#include "EOverlayState.h"
#include "ERotationMode.h"
#include "EStance.h"
#include "EViewMode.h"
#include "IALSCharacter.generated.h"

UINTERFACE(Blueprintable)
class UIALSCharacter : public UInterface {
    GENERATED_BODY()
};

class IIALSCharacter : public IInterface {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void SetViewMode(EViewMode ViewMode);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void SetStance(EStance NewStance);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void SetRotationMode(ERotationMode NewRotationMode);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void SetOverlayState(EOverlayState NewOverLayState);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void SetMovementState(EMovementState NewMovementState);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void SetMovementAction(EMovementAction NewMovementAction);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void SetGait(EGait NewGait);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void GetEssentialValues(FVector& OutVelocity, FVector& OutAcceleration, FVector& OutMovementInput, bool& OutIsMoving, bool& OutHasMovementInput, float& OutSpeed, float& OutMovementInputAmount, FRotator& OutRotator, float& OutAimYawRate);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void GetCurrentStates(TEnumAsByte<EMovementMode>& OutPawnMovementMode, EMovementState& OutMovementState, EMovementState& OutPrevMovementState, EMovementAction& OutMovementAction, ERotationMode& OutRotationMode, EGait& OutActualGait, EStance& OutActualStance, EViewMode& OutViewMode, EOverlayState& OutOverlayState);
    
};

