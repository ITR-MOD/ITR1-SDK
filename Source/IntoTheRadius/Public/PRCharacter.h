#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "ActorVolumeActivationInterface.h"
#include "EAwarenessLevel.h"
#include "ECharCommand.h"
#include "ECharDetailLevel.h"
#include "IBotPawn.h"
#include "ICommandEvents.h"
#include "IHealthStatus.h"
#include "IInstanceID.h"
#include "INPC.h"
#include "NPCGameData.h"
#include "NpcPointQueryResult.h"
#include "PRBasicCharacter.h"
#include "PRNPCInfo.h"
#include "PlayerLightSensitiveInterface.h"
#include "Templates/SubclassOf.h"
#include "PRCharacter.generated.h"

class AAIController;
class AActor;
class UAIEdPerceptionComp;
class UAIPerceptionStimuliSourceComponent;
class UActorComponent;
class UAliveStatsComponent;
class UAudioComponent;
class UBehaviorTree;
class UBlackboardComponent;
class UObject;
class UPRNpcPerceptionComponent;
class UWidgetComponent;

UCLASS(Blueprintable)
class INTOTHERADIUS_API APRCharacter : public APRBasicCharacter, public IINPC, public IIInstanceID, public IIHealthStatus, public IPlayerLightSensitiveInterface, public IIBotPawn, public IICommandEvents, public IActorVolumeActivationInterface {
    GENERATED_BODY()
public:
protected:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    UWidgetComponent* DebugWidget;
    
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    UPRNpcPerceptionComponent* PerceptionComponent;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    UAudioComponent* AudioIdle;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    UAIPerceptionStimuliSourceComponent* AIPerceptionStimuliSource;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    UAIEdPerceptionComp* AIEdPerceptionComponent;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    UAliveStatsComponent* AliveStatsComponent;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FName CommandKeyName;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    UBehaviorTree* BTAsset;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float WalkSpeed;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float IdleTime;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool bLookAroundAfterWalk;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float LootPossibility;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FGameplayTag NPCTag;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    AActor* IdleOrigin;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool bStrictOrigin;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool bDebugInfo;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    UObject* CritComponent;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float IdleWalkRadius;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FName CharName;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float LootPossibilityOverride;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    ECharCommand Command;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FVector POI;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float ValidDistanceToPlayer;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool bEnabled;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FString SpawnerId;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FString CustomUID;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool bAlertCooldown;
    
protected:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    ECharDetailLevel CurrentDetailLevel;
    
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    AAIController* AIC;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    UBlackboardComponent* BB;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool bLerpMovement;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TSubclassOf<AActor> LootItemClass;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FPRNPCInfo NpcInfo;
    
protected:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    UActorComponent* ShootingComponent;
    
public:
    APRCharacter(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable)
    void WalkToLocation(const FVector Location);
    
    UFUNCTION(BlueprintCallable)
    void WalkToActorLocation(AActor* Actor);
    
protected:
    UFUNCTION(BlueprintCallable)
    void SwitchAiDebug();
    
public:
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void SpawnLoot() const;
    
    UFUNCTION(BlueprintCallable)
    void SetupState(const FNPCGameData& NPC_data, TArray<FName> BoolNames, TArray<bool> BoolValues);
    
    UFUNCTION(BlueprintCallable)
    void SetEnableEverything(const bool enable);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void SetEnableAdditionalThings(bool enable);
    
    UFUNCTION(BlueprintCallable)
    void SetCommand(ECharCommand NewCommand, bool Force);
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void SetBrainEnabled(const bool enable) const;
    
    UFUNCTION(BlueprintCallable)
    bool SelectAndAttackClosest();
    
    UFUNCTION(BlueprintCallable)
    bool SelectAndAttack(AActor* Actor);
    
    UFUNCTION(BlueprintCallable)
    void SaveGameData();
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void RestartTree() const;
    
private:
    UFUNCTION(BlueprintCallable)
    void ReappearOutOfSight();
    
    UFUNCTION(BlueprintCallable)
    void ReappearCoverCallback(const FNpcPointQueryResult& QueryResult);
    
public:
    UFUNCTION(BlueprintCallable)
    void ProcessNoise(const FName Tag, const FVector Location);
    
    UFUNCTION(BlueprintCallable)
    void ProcessDamage(AActor* causer, float Damage);
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void PrintMessage(const FString& MessageToPrint) const;
    
    UFUNCTION(BlueprintCallable)
    void OnHeardNoise(AActor* Actor, FVector Location, float Strength, FName Tag);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void OnDeath(bool bEnableRagdoll);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void OnDamaged(AActor* causer, const float Damage);
    
private:
    UFUNCTION(BlueprintCallable)
    void NpcAwarenessLevelChanged(const EAwarenessLevel PrevLevel, const EAwarenessLevel NewLevel);
    
public:
    UFUNCTION(BlueprintCallable)
    void LoadBaseSettings();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void LoadAdditionalSettings();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsPlayerNear() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsLocInValidRange(FVector Location) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsEnabled() const;
    
    UFUNCTION(BlueprintCallable)
    void InvestigateLocation(FVector Location);
    
    UFUNCTION(BlueprintCallable)
    void HealthEnd();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FVector GetIdleOriginPoint() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    float GetFootstepNoiseRange() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    EAwarenessLevel GetAwarenessLevel() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    float GetAttackMoveDilationAngle() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    float GetAttackLostLocationTime() const;
    
    UFUNCTION(BlueprintCallable)
    void EnableRagdoll();
    
    UFUNCTION(BlueprintCallable)
    void DestroyController();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void CharEnabled();
    
    UFUNCTION(BlueprintCallable)
    void AlertNearbyMonsters(AActor* causer, const bool bDeath);
    

    // Fix for true pure virtual functions not being implemented
};

