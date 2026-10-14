#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/NoExportTypes.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "EDamageCategory.h"
#include "EHandPose.h"
#include "EInventoryMode.h"
#include "EPRHand.h"
#include "EPRHandPoseGeometryType.h"
#include "EPRHandPoseNumber.h"
#include "EPlayerTeleportAdjustType.h"
#include "IPRPlayerCharacter.generated.h"

class AActor;
class UCameraComponent;
class UGripMotionControllerComponent;
class UInventoryItem;
class UMaterialInstanceDynamic;
class UObject;
class UPRItemConfig;
class USceneComponent;
class USkeletalMeshComponent;
class USphereComponent;

UINTERFACE(Blueprintable)
class UIPRPlayerCharacter : public UInterface {
    GENERATED_BODY()
};

class IIPRPlayerCharacter : public IInterface {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void UpdateHolders();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void TeleportPlayer(FTransform Transform, FVector Velocity, bool wristMenuHidden, bool ReprojectLocation, EPlayerTeleportAdjustType adjust);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void StopMovement();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void StopHapticses();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void SpawnItemInHand(bool LeftHand, FGameplayTag ItemTypeID);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void SpawnItemConfigInHand(bool bLeftHand, UPRItemConfig* itemConfig);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void SpawnInventory();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void SetWristMenuHidden(bool Hidden);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void SetPaused(bool paused);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool SetInventoryMode(EInventoryMode Mode);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void SetCurrentVelocity(const FVector Velocity);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void ReturnBackpack(bool instant);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void RefreshHandPose(UGripMotionControllerComponent* Controller);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void PreTeleport();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OpenEyes();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnTeleported();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnDominantHandChosen();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool IsTeleporting() const;
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    bool IsItemActorSticked(AActor* Actor);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool IsImmuneToDamage(const EDamageCategory Type) const;
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    bool IsActorLerpingToHand(const UObject* Object);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void Initialize();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    float GetTotalEquipmentWeight();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    FTransform GetRootTransform() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    float GetPlayerHeightScale() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    float GetPlayerHeight() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void GetOneHeldItem(EPRHand hand, const bool onlyItem, UObject*& itemActorObject, bool& SecondaryGrip) const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    EInventoryMode GetInventoryMode() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void GetHeldItems(const bool bOnlyItem, UObject*& Left, UObject*& Right) const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void GetHandsIKData(bool& success, bool& alive, FTransform& leftHandTransform, FTransform& leftHandShiftTransform, EHandPose& leftHandPose, EPRHandPoseGeometryType& leftHandGeometryType, EPRHandPoseNumber& leftHandPoseNumber, UObject*& leftHeldItem, FTransform& rightHandTransform, FTransform& rightHandShiftTransform, EHandPose& rightHandPose, EPRHandPoseGeometryType& rightHandGeometryType, EPRHandPoseNumber& rightHandPoseNumber, UObject*& rightHeldItem);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void GetHandPivot(UGripMotionControllerComponent* Controller, USceneComponent*& Pivot);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    float GetDistanceToCamera(const FVector WorldLocation) const;
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void GetCurrentVelocity(FVector& Velocity) const;
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void GetControlsRefs(bool& success, bool& alive, UCameraComponent*& cameraRef, USceneComponent*& pivotHead, UGripMotionControllerComponent*& leftControllerRef, USceneComponent*& pivotLeftController, USceneComponent*& pivotLeftHand, USphereComponent*& sphereLeftHand, UMaterialInstanceDynamic*& materialLeftController, UGripMotionControllerComponent*& rightControllerRef, USceneComponent*& pivotRightController, USceneComponent*& pivotRightHand, USphereComponent*& sphereRightHand, UMaterialInstanceDynamic*& materialRightController);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    UCameraComponent* GetCameraRef() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    USkeletalMeshComponent* GetBodyRef() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void GetBodyIKData(bool& bSuccess, bool& bAlive, FTransform& RootTransform, FTransform& CameraTransform, FTransform& NeckTransform);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void GetBeamPointerComponents(USceneComponent*& Left, USceneComponent*& Right);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    USceneComponent* GetBackpackPivot();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    TMap<EDamageCategory, float> GetArmorStats() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    TArray<UInventoryItem*> GetArmorItems() const;
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void DropNotItems();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void DropAll();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void CloseEyes();
    
};

