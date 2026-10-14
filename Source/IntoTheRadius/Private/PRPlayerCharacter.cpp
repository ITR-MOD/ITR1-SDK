#include "PRPlayerCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "PRPlayerGripSystemComponent.h"
#include "RadiusAutomationComponent.h"
#include "RadiusPlayerMovementComponent.h"

APRPlayerCharacter::APRPlayerCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->SKBody = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SKBody"));
    this->bSweepTeleport = false;
    this->BpVisibilityComp = NULL;
    this->bStatePaused = false;
    this->bIsCrouchToggled = false;
    this->GripSystemComp = CreateDefaultSubobject<UPRPlayerGripSystemComponent>(TEXT("PlayerGripSystem"));
    this->AutomationComp = CreateDefaultSubobject<URadiusAutomationComponent>(TEXT("Automation"));
    this->RadiusPlayerMovementComponent = CreateDefaultSubobject<URadiusPlayerMovementComponent>(TEXT("Movement"));
    this->ForwardBackwardAxis = 0.00f;
    this->DPadVelocityScaler = 0.00f;
    this->RightLeftAxis = 0.00f;
    this->IsHandClimbing = false;
    this->LThumbPressed = false;
    this->SpeedAlpha = 0.00f;
    this->TotalEquipmentWeight = 0.00f;
    this->Sprinting = false;
    this->RightTriggerAngle = 0.00f;
    this->LeftTriggerAngle = 0.00f;
    this->TriggerUnlockThreshold = 0.00f;
    this->TriggerPressThreshold = 0.00f;
    this->bDeathTriggered = false;
    this->HandPoseRight = EHandPose::Default;
    this->HandPoseLeft = EHandPose::Default;
    this->RUIPressed = false;
    this->LUIPressed = false;
    this->RGripPressed = false;
    this->LGripPressed = false;
    this->BackpackTriggerDistance = 0.00f;
    this->BP_BackpackSimpleClass = NULL;
    this->BP_HolderMarkClass = NULL;
    this->ClimbStartSound = NULL;
    this->ClimbStartSoundAttenuation = NULL;
    this->GrabSound = NULL;
    this->GrabSoundAttenuation = NULL;
    this->backpack = NULL;
    this->SelectorsController = NULL;
    this->ObjectInRightHand = NULL;
    this->ObjectInLeftHand = NULL;
    this->RightHandFakeSecondary = false;
    this->LeftHandFakeSecondary = false;
    this->RightHandStick = false;
    this->LeftHandStick = false;
    this->ClimbingHand = NULL;
    this->bIsObjectRelative = false;
    this->ClimbGripObject = NULL;
    this->GripMover = NULL;
    this->IsFocused = false;
    this->SecondaryGripingController = NULL;
    this->LeftLastHaptic = NULL;
    this->RightLastHaptic = NULL;
    this->TriggerGripRight = NULL;
    this->TriggerGripLeft = NULL;
    this->bZeroVel = false;
    this->bBodyVisibilityFlag = false;
    this->PlayerMeshEyeHeight = 171.00f;
    this->NeckOffset = 17.00f;
    this->PlayerMeshHeight = 183.50f;
    this->MinPlayerHeight = 120.00f;
    this->MaxPlayerHeight = 220.00f;
    this->PlayerStatsComponent = NULL;
    this->SKBody->SetupAttachment(RootComponent);
}


void APRPlayerCharacter::UpdateIndicatorsVisibility_Native() {
}


bool APRPlayerCharacter::TryToGrabObject_Native(UObject* ObjectToTryToGrab, const FTransform& WorldTransform, UGripMotionControllerComponent* CallingHand, UGripMotionControllerComponent* OtherHand, bool bIsSlotGrip, EControllerHand EnumHand, bool bIsDistanceGrip) {
    return false;
}

void APRPlayerCharacter::TryDropSimulating_Native(UGripMotionControllerComponent* hand) {
}

void APRPlayerCharacter::TakeInHand_Native(UGripMotionControllerComponent* hand, UObject* Object, bool bIsCallerHandLeft) {
}

void APRPlayerCharacter::RecreateBackpack_Native() {
}

void APRPlayerCharacter::ProcessDamageArmor(const float Damage, const EDamageCategory DamageCategory, AActor* causer, float& ResultDamage) {
}

void APRPlayerCharacter::PositionClimbingIndicatorNative(bool bIsLeftHand, bool bReset) {
}

void APRPlayerCharacter::OverlapsUINative(const UPrimitiveComponent* Prim, bool bLeft, bool& bOverlaps) const {
}

void APRPlayerCharacter::OverlapGripNative(bool bLeft) {
}




void APRPlayerCharacter::IsCrouch(bool& bCrouch) {
}

bool APRPlayerCharacter::IsAliveNative() {
    return false;
}

void APRPlayerCharacter::InitClimbing_Native(UGripMotionControllerComponent* hand, UObject* Object, bool isObjectRelative) {
}


bool APRPlayerCharacter::GripOrDropObject_Native(UGripMotionControllerComponent* CallingHand, UGripMotionControllerComponent* OtherHand) {
    return false;
}

void APRPlayerCharacter::GripOrDrop_Native(UGripMotionControllerComponent* CallingHand, UGripMotionControllerComponent* OtherHand, const bool bIsGrip, const USphereComponent* GrabSphere) {
}





















void APRPlayerCharacter::GetNearestOverlapObjectNative(USphereComponent* OverlapComponent, UGripMotionControllerComponent* hand, UGripMotionControllerComponent* OtherHand, UObject*& OutNearestObject, bool& bOutImplementsInterface, FTransform& OutObjectTransform, bool& bOutCanBeClimbed, UPrimitiveComponent*& OutPrimitive, bool& bOutIsDistanceGrip, FTransform& OutGripPoint, bool& bOutIsSlotGrip, UPrimitiveComponent*& OutTriggerPrimitive, FTransform& OutTriggerTransform, bool& bOutSecondary, FTransform& OutHandTransformWhenOverlap, EPRHandPoseGeometryType& OutGeometryType, EPRHandPoseNumber& OutPoseNumber) {
}











void APRPlayerCharacter::GetHandDataNative(bool bLeftHand, FTransform& ControllerTransform, FTransform& HandWorldTransform, FTransform& HandShiftTransform, UObject*& Object, bool& SecondaryGrip, EPRHandPoseGeometryType& HandPoseGeometryType, EPRHandPoseNumber& HandPoseNumber) {
}






bool APRPlayerCharacter::DropOrRetakeOrHolster_Native(UGripMotionControllerComponent* CallingHand, UObject* ObjectToGrip, const bool bSecondaryGrip, const bool bZeroVelocity) {
    return false;
}

bool APRPlayerCharacter::DropOrHolster_Native(UGripMotionControllerComponent* CallingHand, UObject* ObjectToGrip, const bool bSecondaryGrip) {
    return false;
}

bool APRPlayerCharacter::DoesPlayerHolsterPreventInteraction(const AActor* Interactable) {
    return false;
}

void APRPlayerCharacter::ClearClimbing_Native(const bool bBecauseOfStepUp) {
}

bool APRPlayerCharacter::CheckTriggerOverlaps_Native(const USphereComponent* GrabSphere, UGripMotionControllerComponent* CallingHand) {
    return false;
}

void APRPlayerCharacter::CheckHolster_Native(UObject* Object, const bool bRemove, const bool bIsLeftHand, bool& bHolsterFound, bool& bHolsterOk, UHolsterComponent*& Holster) {
}



void APRPlayerCharacter::BeamPointerToggleNative(bool bOnOff, FVector TargetPoint, bool bIsLeftHand, bool bUIGrip) {
}


