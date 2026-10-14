#pragma once
#include "CoreMinimal.h"
#include "Perception/AIPerceptionTypes.h"
#include "UObject/NoExportTypes.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "EAwarenessLevel.h"
#include "IBotComponent.h"
#include "NPCAwarenessConfig.h"
#include "NPCPerceptionConfig.h"
#include "OnAwarenessLevelChangedDelegate.h"
#include "OnHeardNoiseDelegate.h"
#include "OnHeardNoiseNewDelegate.h"
#include "OneParamActorDelegate.h"
#include "PRBasicSceneComponent.h"
#include "PRNPCInfo.h"
#include "PlayerLostDelegateDelegate.h"
#include "Templates/SubclassOf.h"
#include "TwoParamsActorVectorDelegate.h"
#include "PRNpcPerceptionComponent.generated.h"

class AAIController;
class AActor;
class APRPlayerCharacter;
class APRPointOfInterest;
class UAIEdPerceptionComp;
class UAIPerceptionComponent;

UCLASS(Blueprintable, ClassGroup=Custom, meta=(BlueprintSpawnableComponent))
class INTOTHERADIUS_API UPRNpcPerceptionComponent : public UPRBasicSceneComponent, public IIBotComponent {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, Transient, meta=(AllowPrivateAccess=true))
    FOneParamActor OnSpottedPlayer;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, Transient, meta=(AllowPrivateAccess=true))
    FPlayerLostDelegate OnLostPlayer;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, Transient, meta=(AllowPrivateAccess=true))
    FOneParamActor OnPlayerSelectionChange;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, Transient, meta=(AllowPrivateAccess=true))
    FTwoParamsActorVector OnPoiChange;
    
    UPROPERTY(BlueprintAssignable, BlueprintCallable, BlueprintReadWrite, EditAnywhere, Transient, meta=(AllowPrivateAccess=true))
    FOnHeardNoise OnHeardNoise;
    
    UPROPERTY(BlueprintAssignable, BlueprintCallable, BlueprintReadWrite, EditAnywhere, Transient, meta=(AllowPrivateAccess=true))
    FOnHeardNoiseNew OnHeardNoiseNew;
    
    UPROPERTY(BlueprintAssignable, BlueprintCallable, BlueprintReadWrite, EditAnywhere, Transient, meta=(AllowPrivateAccess=true))
    FOnAwarenessLevelChanged OnAwarenessLevelChanged;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool bDebugInfoEnabled;
    
protected:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TSubclassOf<APRPointOfInterest> PoiActorClass;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TSubclassOf<UAIPerceptionComponent> AIEdPerceptionComponentClass;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float SenseDurationSpottedThreshold;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float SenseDurationLostThreshold;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool bForgetDead;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool bPoiFollowSelection;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float DebugHeight;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    UAIEdPerceptionComp* AIPerceptionComponent;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FNPCPerceptionConfig PerceptionConfig;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FNPCAwarenessConfig AwarenessConfig;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FGameplayTag NPCConfigID;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FPRNPCInfo NpcInfo;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FString OwnerName;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    AAIController* AIC;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    APRPointOfInterest* NpcPoiActor;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    APRPlayerCharacter* PlayerRef;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    AActor* CurrentPlayerSelection;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float SenseTickTime;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float SightSenseRadius;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float SightSenseNightModifier;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float SightSenseAngleLeft;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float SightSenseAngleRight;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float SightSenseAngleUp;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float SightSenseAngleDown;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float HearingDistance;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float AwarenessLowValueLevel;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float AwarenessHighValueLevel;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float AwarenessMaxValue;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float AwarenessDecreaseMultiplier;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    EAwarenessLevel AwarenessLevel;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float AwarenessValue;
    
public:
    UPRNpcPerceptionComponent(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable)
    void VerifyPlayerSelection();
    
    UFUNCTION(BlueprintCallable)
    void UnselectPlayer();
    
    UFUNCTION(BlueprintCallable)
    void SpotPlayer();
    
    UFUNCTION(BlueprintCallable)
    void SpotActor(const AActor* Actor);
    
    UFUNCTION(BlueprintCallable)
    void SetupInitialState(float InitialAwareness, bool bPlayerInitiallySelected);
    
    UFUNCTION(BlueprintCallable)
    void SetSelection(const AActor* Actor, bool& bSuccess, AActor*& Selection);
    
    UFUNCTION(BlueprintCallable)
    void SetPoiFollowSelection(const bool bShouldFollow);
    
    UFUNCTION(BlueprintCallable)
    void SetPoi(const FVector& NewPoi, bool& bChanged, AActor*& POIActor);
    
    UFUNCTION(BlueprintCallable)
    void SelectPlayer();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void SelectionIsAlive(bool& bAlive) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void IsSpotted(const AActor* Actor, bool& bSpotted) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void IsPlayerSpotted(bool& bSpotted) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void IsPlayerSelectionAlive(bool& bAlive) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void IsEnemy(const AActor* Actor, bool& bIsEnemy) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void IsActorSpottable(const AActor* Actor, bool& bSpottable) const;
    
    UFUNCTION(BlueprintCallable)
    void HaveSelection(bool& bValid);
    
    UFUNCTION(BlueprintCallable)
    void GetSensedPlayerLocation(bool& bValid, FVector& Location);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetSelection(bool& bValid, AActor*& Selection, FTransform& BodyTransform) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetPoi(bool& bValid, AActor*& POIActor);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetPlayerSelection(bool& bValid, AActor*& Player, FTransform& BodyTransform) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetClosestActor(bool& bSuccess, AActor*& Actor) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetAwareness(float& Value, EAwarenessLevel& Level) const;
    
    UFUNCTION(BlueprintCallable)
    void ChangeAwarenessByTag(float Strength, FName Tag);
    
    UFUNCTION(BlueprintCallable)
    void ChangeAwareness(float Value);
    
private:
    UFUNCTION(BlueprintCallable)
    void AIPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus);
    

    // Fix for true pure virtual functions not being implemented
};

