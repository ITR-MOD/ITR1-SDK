#include "PRALSCharacter.h"

APRALSCharacter::APRALSCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->Speed = 0.00f;
    this->IsMoving = false;
    this->HasMovementInput = false;
    this->MovementInputAmount = 0.00f;
    this->AimYawRate = 0.00f;
    this->OptimalShootRange = 0.00f;
    this->BotMovementMode = EBotMovementMode::WalkIdle;
    this->PreviousAimYaw = 0.00f;
    this->DesiredRotationMode = ERotationMode::LookingDirection;
    this->DesiredGait = EGait::Running;
    this->DesiredStance = EStance::Standing;
    this->LookUpDownRate = 1.25f;
    this->LookLeftRightRate = 1.25f;
    this->TimesPressedStance = 0;
    this->BreakFall = false;
    this->SprintHeld = false;
    this->MainAnimInstance = NULL;
    this->MovementState = EMovementState::None;
    this->PrevMovementState = EMovementState::None;
    this->RotationMode = ERotationMode::VelocityDirection;
    this->Gait = EGait::Walking;
    this->Stance = EStance::Standing;
    this->OverlayState = EOverlayState::Default;
    this->ViewMode = EViewMode::ThirdPerson;
    this->YawOffset = 0.00f;
    this->RagdollOnGround = false;
    this->RagdollFaceUp = false;
    this->DeathSound = NULL;
    this->DeathParticle = NULL;
}

void APRALSCharacter::UpdateGroundedRotation() {
}

void APRALSCharacter::UpdateDynamicMovementSettings(EGait AllowedGait) {
}

void APRALSCharacter::UpdateCharacterMovement() {
}

void APRALSCharacter::SmoothCharacterRotation(FRotator Target, float TargetInterpSpeed, float ActorInterpSpeed) {
}

void APRALSCharacter::SetMovementModel() {
}

void APRALSCharacter::SetEssentialValues() {
}

void APRALSCharacter::SetActorLocationDuringRagdoll() {
}

bool APRALSCharacter::SetActorLocationAndRotation(FVector NewLocation, FRotator NewRotation, bool bSweep, bool bTeleport, FHitResult& OutSweepHitResult) {
    return false;
}

void APRALSCharacter::RagdollUpdate() {
}

void APRALSCharacter::RagdollStart() {
}

void APRALSCharacter::RagdollEnd() {
}

void APRALSCharacter::OnViewModeChanged(EViewMode NewViewMode) {
}

void APRALSCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) {
}

void APRALSCharacter::OnStanceChanged(EStance NewStance) {
}

void APRALSCharacter::OnRotationModeChanged(ERotationMode NewRotationMode) {
}

void APRALSCharacter::OnOverlayStateChanged(EOverlayState NewOverLayState) {
}

void APRALSCharacter::OnMovementStateChanged(EMovementState NewMovementState) {
}

void APRALSCharacter::OnGaitChanged(EGait NewActualGait) {
}

void APRALSCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) {
}

void APRALSCharacter::OnCharacterMovementModeChanged(TEnumAsByte<EMovementMode> NewMovementMode, uint8 NewCustomMode) {
}

void APRALSCharacter::OnCharacterMovementActionChanged(EMovementAction NewMovementAction) {
}

void APRALSCharacter::OnBeginPlay() {
}

void APRALSCharacter::LimitRotation(float AimYawMin, float AimYawMax, float InterpSpeed) {
}

FMovementSettings APRALSCharacter::GetTargetMovementSettings() const {
    return FMovementSettings{};
}

UAnimMontage* APRALSCharacter::GetRollAnimation() const {
    return NULL;
}

float APRALSCharacter::GetMappedSpeed() const {
    return 0.0f;
}

UAnimMontage* APRALSCharacter::GetGetUpAnimation(bool bRagDollFaceUp) {
    return NULL;
}

FVector APRALSCharacter::GetCapsuleLocationFromBase(FVector BaseLocation, float ZOffset) const {
    return FVector{};
}

FVector APRALSCharacter::GetCapsuleBaseLocation(float ZOffset) const {
    return FVector{};
}

float APRALSCharacter::GetAnimCurveValue(FName CurveName) const {
    return 0.0f;
}

FRotator APRALSCharacter::GetAimingRotation() const {
    return FRotator{};
}

EGait APRALSCharacter::GetActualGait(EGait AllowedGait) const {
    return EGait::Walking;
}

bool APRALSCharacter::CanUpdateMovingRotation() const {
    return false;
}

bool APRALSCharacter::CanSprint() const {
    return false;
}

float APRALSCharacter::CalculateGroundedRotationRate() const {
    return 0.0f;
}

FVector APRALSCharacter::CalculateAcceleration() const {
    return FVector{};
}

void APRALSCharacter::CacheValues() {
}

void APRALSCharacter::BeginPlay() {
}

void APRALSCharacter::AddToCharacterRotation(FRotator DeltaRotation) {
}


