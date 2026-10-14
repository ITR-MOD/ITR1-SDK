#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/NoExportTypes.h"
#include "InputCoreTypes.h"
#include "VRCharacter.h"
#include "EDamageCategory.h"
#include "EHandPose.h"
#include "EHapticsCurve.h"
#include "EInventoryMode.h"
#include "EPRHandPoseGeometryType.h"
#include "EPRHandPoseNumber.h"
#include "EPRHandPoseType.h"
#include "ESpeedCoefficient.h"
#include "GripOverlap.h"
#include "IHealthStatus.h"
#include "IPRPlayerCharacter.h"
#include "PRPlayerControlsInterface.h"
#include "PRPlayerPawn.h"
#include "Templates/SubclassOf.h"
#include "PRPlayerCharacter.generated.h"

class AActor;
class ABackpackSimple;
class APRHolderMark;
class UBoxComponent;
class UCapsuleComponent;
class UChildActorComponent;
class UGripMotionControllerComponent;
class UHolsterComponent;
class UObject;
class UPRPlayerGripSystemComponent;
class UPRPlayerVisibilityComponent;
class UParticleSystemComponent;
class UPlayerStatsComponent;
class UPrimitiveComponent;
class URadiusAutomationComponent;
class URadiusPlayerMovementComponent;
class USceneComponent;
class USkeletalMeshComponent;
class USoundAttenuation;
class USoundBase;
class USphereComponent;
class UStaticMeshComponent;
class UVRControllerComponent;
class UWidgetComponent;
class UWidgetInteractionComponent;

UCLASS(Blueprintable)
class INTOTHERADIUS_API APRPlayerCharacter : public AVRCharacter, public IIPRPlayerCharacter, public IIHealthStatus, public IPRPlayerPawn, public IPRPlayerControlsInterface {
    GENERATED_BODY()
public:
protected:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    USkeletalMeshComponent* SKBody;
    
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool bSweepTeleport;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    UPRPlayerVisibilityComponent* BpVisibilityComp;
    
protected:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool bStatePaused;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool bIsCrouchToggled;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    UPRPlayerGripSystemComponent* GripSystemComp;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    URadiusAutomationComponent* AutomationComp;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    URadiusPlayerMovementComponent* RadiusPlayerMovementComponent;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<ESpeedCoefficient, float> MoveSpeedMult;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FVector MovementVector;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float ForwardBackwardAxis;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float DPadVelocityScaler;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float RightLeftAxis;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool IsHandClimbing;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool LThumbPressed;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float SpeedAlpha;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float TotalEquipmentWeight;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool Sprinting;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float RightTriggerAngle;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float LeftTriggerAngle;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float TriggerUnlockThreshold;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float TriggerPressThreshold;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool bDeathTriggered;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FGripOverlap OverlapGripRight;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FGripOverlap OverlapGripLeft;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FGripOverlap SelectedGripTargetRight;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FGripOverlap SelectedGripTargetLeft;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    EHandPose HandPoseRight;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    EHandPose HandPoseLeft;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool RUIPressed;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool LUIPressed;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool RGripPressed;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool LGripPressed;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float BackpackTriggerDistance;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TSubclassOf<ABackpackSimple> BP_BackpackSimpleClass;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TSubclassOf<APRHolderMark> BP_HolderMarkClass;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    USoundBase* ClimbStartSound;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    USoundAttenuation* ClimbStartSoundAttenuation;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    USoundBase* GrabSound;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    USoundAttenuation* GrabSoundAttenuation;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    ABackpackSimple* backpack;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    UObject* SelectorsController;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    UObject* ObjectInRightHand;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    UObject* ObjectInLeftHand;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool RightHandFakeSecondary;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool LeftHandFakeSecondary;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool RightHandStick;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool LeftHandStick;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    UGripMotionControllerComponent* ClimbingHand;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FVector ClimbGripLocation;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool bIsObjectRelative;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    UPrimitiveComponent* ClimbGripObject;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    AActor* GripMover;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool IsFocused;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FVector DistanceGripPoint_R;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FVector DistanceGripPoint_L;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    USceneComponent* SecondaryGripingController;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    AActor* LeftLastHaptic;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    AActor* RightLastHaptic;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    UObject* TriggerGripRight;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    UObject* TriggerGripLeft;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<UHolsterComponent*, APRHolderMark*> HolsterMarks;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool bZeroVel;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool bBodyVisibilityFlag;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float PlayerMeshEyeHeight;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float NeckOffset;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float PlayerMeshHeight;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float MinPlayerHeight;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float MaxPlayerHeight;
    
private:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    UPlayerStatsComponent* PlayerStatsComponent;
    
public:
    APRPlayerCharacter(const FObjectInitializer& ObjectInitializer);

protected:
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void UpdateItemInfos();
    
    UFUNCTION(BlueprintCallable)
    void UpdateIndicatorsVisibility_Native();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, BlueprintPure=false)
    void UnbindInteractable(const AActor* Actor) const;
    
    UFUNCTION(BlueprintCallable)
    bool TryToGrabObject_Native(UObject* ObjectToTryToGrab, const FTransform& WorldTransform, UGripMotionControllerComponent* CallingHand, UGripMotionControllerComponent* OtherHand, bool bIsSlotGrip, EControllerHand EnumHand, bool bIsDistanceGrip);
    
    UFUNCTION(BlueprintCallable)
    void TryDropSimulating_Native(UGripMotionControllerComponent* hand);
    
    UFUNCTION(BlueprintCallable)
    void TakeInHand_Native(UGripMotionControllerComponent* hand, UObject* Object, bool bIsCallerHandLeft);
    
    UFUNCTION(BlueprintCallable)
    void RecreateBackpack_Native();
    
    UFUNCTION(BlueprintCallable)
    void ProcessDamageArmor(const float Damage, const EDamageCategory DamageCategory, AActor* causer, float& ResultDamage);
    
    UFUNCTION(BlueprintCallable)
    void PositionClimbingIndicatorNative(bool bIsLeftHand, bool bReset);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void OverlapsUINative(const UPrimitiveComponent* Prim, bool bLeft, bool& bOverlaps) const;
    
    UFUNCTION(BlueprintCallable)
    void OverlapGripNative(bool bLeft);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnInventoryModeChanged(const EInventoryMode Mode);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void LerpRightGrip(AActor* Actor, UGripMotionControllerComponent* hand, bool bIsSlot, FTransform Offset, bool bDisableCollision, bool bKeepHandSnapped);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void LerpLeftGrip(AActor* Actor, UGripMotionControllerComponent* hand, bool bIsSlot, FTransform Offset, bool bDisableCollision, bool bKeepHandSnapped);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void IsCrouch(bool& bCrouch);
    
public:
    UFUNCTION(BlueprintCallable)
    bool IsAliveNative();
    
protected:
    UFUNCTION(BlueprintCallable)
    void InitClimbing_Native(UGripMotionControllerComponent* hand, UObject* Object, bool isObjectRelative);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void HandHaptics(EHapticsCurve Type, EControllerHand hand);
    
    UFUNCTION(BlueprintCallable)
    bool GripOrDropObject_Native(UGripMotionControllerComponent* CallingHand, UGripMotionControllerComponent* OtherHand);
    
    UFUNCTION(BlueprintCallable)
    void GripOrDrop_Native(UGripMotionControllerComponent* CallingHand, UGripMotionControllerComponent* OtherHand, const bool bIsGrip, const USphereComponent* GrabSphere);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, BlueprintPure)
    UWidgetInteractionComponent* GetWidgetInteractionRight() const;
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, BlueprintPure)
    UWidgetInteractionComponent* GetWidgetInteractionLeft() const;
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, BlueprintPure)
    UWidgetComponent* GetW_R_WristMenu() const;
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, BlueprintPure)
    UWidgetComponent* GetW_L_WristMenu() const;
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, BlueprintPure)
    UCapsuleComponent* GetUIPoseTriggerRight() const;
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, BlueprintPure)
    UCapsuleComponent* GetUIPoseTriggerLeft() const;
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, BlueprintPure)
    UBoxComponent* GetTriggerWeaponSecondary() const;
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, BlueprintPure)
    UBoxComponent* GetTriggerWeaponPrimary() const;
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, BlueprintPure)
    UBoxComponent* GetTriggerMelee() const;
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, BlueprintPure)
    UBoxComponent* GetTriggerMap() const;
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, BlueprintPure)
    UBoxComponent* GetTriggerDetector() const;
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, BlueprintPure)
    UBoxComponent* GetTriggerBackpack() const;
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, BlueprintPure)
    UStaticMeshComponent* GetSM_ClimbingIndicatorRight() const;
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, BlueprintPure)
    UStaticMeshComponent* GetSM_ClimbingIndicatorLeft() const;
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, BlueprintPure)
    USphereComponent* GetRHandCollider() const;
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, BlueprintPure)
    UParticleSystemComponent* GetPS_BeamPointerRight() const;
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, BlueprintPure)
    UParticleSystemComponent* GetPS_BeamPointerLeft() const;
    
public:
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, BlueprintPure)
    USceneComponent* GetPivotVest() const;
    
protected:
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, BlueprintPure)
    USceneComponent* GetPivotNeck() const;
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, BlueprintPure)
    USceneComponent* GetPivotBackpack() const;
    
    UFUNCTION(BlueprintCallable)
    void GetNearestOverlapObjectNative(USphereComponent* OverlapComponent, UGripMotionControllerComponent* hand, UGripMotionControllerComponent* OtherHand, UObject*& OutNearestObject, bool& bOutImplementsInterface, FTransform& OutObjectTransform, bool& bOutCanBeClimbed, UPrimitiveComponent*& OutPrimitive, bool& bOutIsDistanceGrip, FTransform& OutGripPoint, bool& bOutIsSlotGrip, UPrimitiveComponent*& OutTriggerPrimitive, FTransform& OutTriggerTransform, bool& bOutSecondary, FTransform& OutHandTransformWhenOverlap, EPRHandPoseGeometryType& OutGeometryType, EPRHandPoseNumber& OutPoseNumber);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, BlueprintPure)
    USphereComponent* GetLHandCollider() const;
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, BlueprintPure)
    bool GetIsInteractableHolsterable(AActor* Actor) const;
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, BlueprintPure)
    UHolsterComponent* GetHolsterWeaponSecondary() const;
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, BlueprintPure)
    UHolsterComponent* GetHolsterWeaponPrimary() const;
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, BlueprintPure)
    UHolsterComponent* GetHolsterProbes_R() const;
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, BlueprintPure)
    UHolsterComponent* GetHolsterProbes_L() const;
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, BlueprintPure)
    UHolsterComponent* GetHolsterMelee() const;
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, BlueprintPure)
    UHolsterComponent* GetHolsterMap() const;
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, BlueprintPure)
    UHolsterComponent* GetHolsterDetector() const;
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, BlueprintPure)
    UChildActorComponent* GetHolderSmallPouchActorComponent() const;
    
    UFUNCTION(BlueprintCallable)
    void GetHandDataNative(bool bLeftHand, FTransform& ControllerTransform, FTransform& HandWorldTransform, FTransform& HandShiftTransform, UObject*& Object, bool& SecondaryGrip, EPRHandPoseGeometryType& HandPoseGeometryType, EPRHandPoseNumber& HandPoseNumber);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, BlueprintPure)
    void GetGripRelativeTransformByGeometryTypeAndPoseNumber(UObject* GripObject, UGripMotionControllerComponent* HandController, EPRHandPoseType PoseType, EPRHandPoseGeometryType GeometryType, EPRHandPoseNumber PoseNumber, bool& bSuccess, FTransform& GripRelativeTransform) const;
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, BlueprintPure)
    USphereComponent* GetGrabSphereRight() const;
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, BlueprintPure)
    USphereComponent* GetGrabSphereLeft() const;
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, BlueprintPure)
    UVRControllerComponent* GetBPC_VRController_Right() const;
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, BlueprintPure)
    UVRControllerComponent* GetBPC_VRController_Left() const;
    
    UFUNCTION(BlueprintCallable)
    bool DropOrRetakeOrHolster_Native(UGripMotionControllerComponent* CallingHand, UObject* ObjectToGrip, const bool bSecondaryGrip, const bool bZeroVelocity);
    
    UFUNCTION(BlueprintCallable)
    bool DropOrHolster_Native(UGripMotionControllerComponent* CallingHand, UObject* ObjectToGrip, const bool bSecondaryGrip);
    
public:
    UFUNCTION(BlueprintCallable)
    bool DoesPlayerHolsterPreventInteraction(const AActor* Interactable);
    
protected:
    UFUNCTION(BlueprintCallable)
    void ClearClimbing_Native(const bool bBecauseOfStepUp);
    
    UFUNCTION(BlueprintCallable)
    bool CheckTriggerOverlaps_Native(const USphereComponent* GrabSphere, UGripMotionControllerComponent* CallingHand);
    
    UFUNCTION(BlueprintCallable)
    void CheckHolster_Native(UObject* Object, const bool bRemove, const bool bIsLeftHand, bool& bHolsterFound, bool& bHolsterOk, UHolsterComponent*& Holster);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void CallNightGlow(UObject* Target, bool bOn);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, BlueprintPure=false)
    void BindInteractable(const AActor* Actor) const;
    
    UFUNCTION(BlueprintCallable)
    void BeamPointerToggleNative(bool bOnOff, FVector TargetPoint, bool bIsLeftHand, bool bUIGrip);
    

    // Fix for true pure virtual functions not being implemented
};

