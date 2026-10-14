#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/NoExportTypes.h"
#include "UObject/NoExportTypes.h"
#include "UObject/NoExportTypes.h"
#include "EBotMovementMode.h"
#include "IBotPawn.generated.h"

class AActor;
class UActorComponent;
class UPRNpcPerceptionComponent;
class USplineComponent;

UINTERFACE(Blueprintable, MinimalAPI)
class UIBotPawn : public UInterface {
    GENERATED_BODY()
};

class IIBotPawn : public IInterface {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void TeleportToNextPos();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void StopMovement();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void SetTargetRotation(FRotator TargetRotation);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void SetFlyForce(FVector Force);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void SetBotMovementMode(EBotMovementMode MovementMode, bool& Changed);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void ReportDeath(AActor* DeadActorRef);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void OnExitPlayerViewport();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void OnEnterPlayerViewport();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    float GetStealth() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    AActor* GetSelection();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    UPRNpcPerceptionComponent* GetPerception();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void GetPatrolTargets(bool& IsValidOut, TArray<AActor*>& TargetsOut) const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    USplineComponent* GetPatrolPath();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    float GetOptimalRange();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    float GetIdleWalkDistance() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    FVector GetIdleOrigin();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    UActorComponent* GetBotShootingComponent();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    EBotMovementMode GetBotMovementMode() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    FTransform GetBoneTransform(FName BoneName) const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    UActorComponent* GetAliveStats();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void CcmStopWaitingForCombat();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void CcmStartWaitingForCombat();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void BroadcastEnemyLoc(const FVector Location, const bool bIsDead);
    
};

