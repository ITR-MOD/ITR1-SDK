#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "Engine/EngineTypes.h"
#include "Engine/EngineTypes.h"
#include "EBotMovementMode.h"
#include "EGait.h"
#include "EMovementAction.h"
#include "EMovementState.h"
#include "EOverlayState.h"
#include "ERotationMode.h"
#include "EStance.h"
#include "EViewMode.h"
#include "IALSAnimation.h"
#include "IALSCharacter.h"
#include "MovementSettings.h"
#include "MovementSettingsState.h"
#include "PRCharacter.h"
#include "PRALSCharacter.generated.h"

class UAnimInstance;
class UAnimMontage;
class UParticleSystem;
class USoundBase;

UCLASS(Blueprintable)
class INTOTHERADIUS_API APRALSCharacter : public APRCharacter, public IIALSAnimation, public IIALSCharacter {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float Speed;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FVector Acceleration;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool IsMoving;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool HasMovementInput;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FRotator LastVelocityRotation;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FRotator LastMovementInputRotation;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float MovementInputAmount;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float AimYawRate;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float OptimalShootRange;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    EBotMovementMode BotMovementMode;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FVector PreviousVelocity;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float PreviousAimYaw;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    ERotationMode DesiredRotationMode;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    EGait DesiredGait;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    EStance DesiredStance;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float LookUpDownRate;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float LookLeftRightRate;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    int32 TimesPressedStance;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool BreakFall;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool SprintHeld;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    UAnimInstance* MainAnimInstance;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    EMovementState MovementState;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    EMovementState PrevMovementState;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    ERotationMode RotationMode;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    EGait Gait;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    EStance Stance;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    EOverlayState OverlayState;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    EViewMode ViewMode;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FDataTableRowHandle MovementModel;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FMovementSettingsState MovementData;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FMovementSettings CurrentMovementSettings;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FRotator TargetRotation;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float YawOffset;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool RagdollOnGround;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool RagdollFaceUp;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FVector LastRagdollVelocity;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    USoundBase* DeathSound;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    UParticleSystem* DeathParticle;
    
    APRALSCharacter(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable)
    void UpdateGroundedRotation();
    
    UFUNCTION(BlueprintCallable)
    void UpdateDynamicMovementSettings(EGait AllowedGait);
    
    UFUNCTION(BlueprintCallable)
    void UpdateCharacterMovement();
    
    UFUNCTION(BlueprintCallable)
    void SmoothCharacterRotation(FRotator Target, float TargetInterpSpeed, float ActorInterpSpeed);
    
    UFUNCTION(BlueprintCallable)
    void SetMovementModel();
    
    UFUNCTION(BlueprintCallable)
    void SetEssentialValues();
    
    UFUNCTION(BlueprintCallable)
    void SetActorLocationDuringRagdoll();
    
    UFUNCTION(BlueprintCallable)
    bool SetActorLocationAndRotation(FVector NewLocation, FRotator NewRotation, bool bSweep, bool bTeleport, FHitResult& OutSweepHitResult);
    
    UFUNCTION(BlueprintCallable)
    void RagdollUpdate();
    
    UFUNCTION(BlueprintCallable)
    void RagdollStart();
    
    UFUNCTION(BlueprintCallable)
    void RagdollEnd();
    
    UFUNCTION(BlueprintCallable)
    void OnViewModeChanged(EViewMode NewViewMode);
    
    UFUNCTION(BlueprintCallable)
    void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust);
    
    UFUNCTION(BlueprintCallable)
    void OnStanceChanged(EStance NewStance);
    
    UFUNCTION(BlueprintCallable)
    void OnRotationModeChanged(ERotationMode NewRotationMode);
    
    UFUNCTION(BlueprintCallable)
    void OnOverlayStateChanged(EOverlayState NewOverLayState);
    
    UFUNCTION(BlueprintCallable)
    void OnMovementStateChanged(EMovementState NewMovementState);
    
    UFUNCTION(BlueprintCallable)
    void OnGaitChanged(EGait NewActualGait);
    
    UFUNCTION(BlueprintCallable)
    void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust);
    
    UFUNCTION(BlueprintCallable)
    void OnCharacterMovementModeChanged(TEnumAsByte<EMovementMode> NewMovementMode, uint8 NewCustomMode);
    
    UFUNCTION(BlueprintCallable)
    void OnCharacterMovementActionChanged(EMovementAction NewMovementAction);
    
    UFUNCTION(BlueprintCallable)
    void OnBeginPlay();
    
    UFUNCTION(BlueprintCallable)
    void LimitRotation(float AimYawMin, float AimYawMax, float InterpSpeed);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FMovementSettings GetTargetMovementSettings() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    UAnimMontage* GetRollAnimation() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    float GetMappedSpeed() const;
    
    UFUNCTION(BlueprintCallable)
    UAnimMontage* GetGetUpAnimation(bool bRagDollFaceUp);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FVector GetCapsuleLocationFromBase(FVector BaseLocation, float ZOffset) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FVector GetCapsuleBaseLocation(float ZOffset) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    float GetAnimCurveValue(FName CurveName) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FRotator GetAimingRotation() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    EGait GetActualGait(EGait AllowedGait) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool CanUpdateMovingRotation() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool CanSprint() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    float CalculateGroundedRotationRate() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FVector CalculateAcceleration() const;
    
    UFUNCTION(BlueprintCallable)
    void CacheValues();
    
    UFUNCTION(BlueprintCallable)
    void BeginPlay();
    
    UFUNCTION(BlueprintCallable)
    void AddToCharacterRotation(FRotator DeltaRotation);
    

    // Fix for true pure virtual functions not being implemented
};

