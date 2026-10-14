#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/NoExportTypes.h"
#include "VRBPDatatypes.h"
#include "PRPlayerPawn.generated.h"

class ABackpackSimple;
class UGripMotionControllerComponent;
class UPrimitiveComponent;

UINTERFACE(Blueprintable, MinimalAPI)
class UPRPlayerPawn : public UInterface {
    GENERATED_BODY()
};

class IPRPlayerPawn : public IInterface {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void SwitchBodyVisibility(const bool bVisible, const bool bRecursive);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void ProcessMovementNative(const float DeltaSeconds);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    bool IsSecondaryGrip(const bool bLeft);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool IsBackpackInHands(const bool bCheckLeft, const bool bCheckRight);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool IsBackpackDropped();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    bool IsAnyLampEnabled();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    TArray<UPrimitiveComponent*> GetVisibilityPrimitives();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    float GetLightningCompensation();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    FVector GetInputMovementVector();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    float GetCrouch();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    ABackpackSimple* GetBackpackRef();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void DropGrip(UGripMotionControllerComponent* GripMotionController, const FBPActorGripInformation& GripToDrop, FVector AngleVel, FVector LinearVel);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void DropAllGrips(UGripMotionControllerComponent* GripMotionController, bool bZeroVelo);
    
};

