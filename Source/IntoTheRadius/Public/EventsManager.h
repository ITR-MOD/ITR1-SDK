#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "DebugEventDelegate.h"
#include "IClearable.h"
#include "NoParamsDelegateDelegate.h"
#include "OneParamActorDelegate.h"
#include "OneParamBoolDelegate.h"
#include "OneParamEnvironmentDelegate.h"
#include "OneParamFloatDelegateDelegate.h"
#include "OneParamIntDelegate.h"
#include "OneParamItemDelegate.h"
#include "OneParamItemsArrayDelegate.h"
#include "OneParamMissionDelegate.h"
#include "OneParamTagDelegate.h"
#include "ThreeParamsGTagIntAndStringsArrayDelegate.h"
#include "ThreeParamsGameSaveDelegate.h"
#include "TwoParamsActorGTagDelegate.h"
#include "TwoParamsGTagsDelegateDelegate.h"
#include "TwoParamsIntAndStringArrayDelegate.h"
#include "TwoParamsMissionAndObjectiveDelegate.h"
#include "TwoParamsSleepDelegate.h"
#include "EventsManager.generated.h"

class AActor;
class APREnvironment;
class UEventsManager;
class UInventoryItem;
class UMissionAbstractObjectiveData;
class UMissionData;

UCLASS(Blueprintable)
class INTOTHERADIUS_API UEventsManager : public UObject, public IIClearable {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FNoParamsDelegate OnGameInstanceStarted;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FNoParamsDelegate OnGameModeStarted;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FNoParamsDelegate OnPlayerStarted;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FNoParamsDelegate OnGameStarted;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FNoParamsDelegate OnGameShutdown;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FNoParamsDelegate OnGameDataSave;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FThreeParamsGameSave OnGameDataSaveDone;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOneParamFloatDelegate OnGameTimeChanged;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOneParamFloatDelegate OnGameplayTimeDilationChanged;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOneParamBool OnGamePaused;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FNoParamsDelegate OnReferencesClearing;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOneParamEnvironment OnEnvironmentRePrepared;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FNoParamsDelegate OnTransitionStart;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FNoParamsDelegate OnTransitionStartOnce;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FNoParamsDelegate OnTransitionEnd;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FNoParamsDelegate OnTransitionEndOnce;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOneParamTag OnLevelPrepare;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOneParamTag OnLevelPrepareOnce;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOneParamTag OnLevelStarted;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOneParamTag OnLevelStartedOnce;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOneParamActor OnItemDropped;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOneParamItem OnItemTaken;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOneParamItem OnItemStored;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOneParamItemsArray OnItemsSold;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOneParamItem OnItemBought;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FTwoParamsIntAndStringArray OnPlayerMoneyChanged;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOneParamItem OnItemChangeDurability;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOneParamInt OnPlayerLevelChanged;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FNoParamsDelegate OnInventoryModeChanged;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOneParamItem OnItemRemoved;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOneParamItem OnItemActorRefRemoved;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOneParamTag OnFirstTimeTaken;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FTwoParamsGTagsDelegate OnItemUpdated;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FThreeParamsGTagIntAndStringsArray OnHolderFixed;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOneParamMission OnNewMissionAdded;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOneParamMission OnMissionPreview;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOneParamMission OnMissionFinished;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOneParamMission OnMissionStateChanged;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FTwoParamsMissionAndObjective OnObjectiveFinished;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FTwoParamsMissionAndObjective OnObjectiveRestored;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FTwoParamsMissionAndObjective OnObjectiveProgress;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOneParamActor OnTutorialSequenceFinished;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FTwoParamsActorGTag OnPlayerKilledNPC;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOneParamActor OnNPCDied;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOneParamActor OnWildSpawnerCleared;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOneParamActor OnPictureTaken;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOneParamActor OnStashOpened;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOneParamTag OnTriggerPassed;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOneParamTag OnTriggerUnset;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOneParamTag OnStoryNotePassed;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOneParamTag OnCustomMapMarkPassed;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOneParamTag OnNewJournalNote;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOneParamItem OnConsumableUsing;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FNoParamsDelegate OnShotFired;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOneParamTag OnTutorialStarted;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FNoParamsDelegate OnTideHappening;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FNoParamsDelegate OnDifficultyChanged;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FNoParamsDelegate OnVirtualStockChanged;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FNoParamsDelegate OnShowBodyChanged;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FNoParamsDelegate OnLanguageChanged;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FNoParamsDelegate OnPlayerReset;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FNoParamsDelegate OnPlayerDied;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOneParamBool OnPlayerEyesOpen;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FTwoParamsSleep OnPlayerSleep;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FNoParamsDelegate OnPlayerTurning;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FNoParamsDelegate OnPlayerWalking;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FNoParamsDelegate OnPlayerRunning;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOneParamBool OnPlayerBodyVisibilityChanged;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FDebugEvent OnSwitchAiCoverDebug;
    
    UEventsManager();

    UFUNCTION(BlueprintCallable, BlueprintPure)
    static UEventsManager* GetEventsManager();
    
    UFUNCTION(BlueprintCallable)
    void FireOnWildSpawnerCleared(AActor* SpawnerActor);
    
    UFUNCTION(BlueprintCallable)
    void FireOnVirtualStockChanged();
    
    UFUNCTION(BlueprintCallable)
    void FireOnTutorialStarted(FGameplayTag TutorialId);
    
    UFUNCTION(BlueprintCallable)
    void FireOnTutorialSequenceFinished(AActor* SequenceActor);
    
    UFUNCTION(BlueprintCallable)
    void FireOnTriggerUnset(FGameplayTag Trigger);
    
    UFUNCTION(BlueprintCallable)
    void FireOnTriggerPassed(FGameplayTag Trigger);
    
    UFUNCTION(BlueprintCallable)
    void FireOnTransitionStart();
    
    UFUNCTION(BlueprintCallable)
    void FireOnTransitionEnd();
    
    UFUNCTION(BlueprintCallable)
    void FireOnTideHappening();
    
    UFUNCTION(BlueprintCallable)
    void FireOnSwitchAiCoverDebug();
    
    UFUNCTION(BlueprintCallable)
    void FireOnStoryNotePassed(FGameplayTag NoteID);
    
    UFUNCTION(BlueprintCallable)
    void FireOnStashOpened(AActor* StashActor);
    
    UFUNCTION(BlueprintCallable)
    void FireOnShowBodyChanged();
    
    UFUNCTION(BlueprintCallable)
    void FireOnShotFired();
    
    UFUNCTION(BlueprintCallable)
    void FireOnReferencesClearing();
    
    UFUNCTION(BlueprintCallable)
    void FireOnPlayerWalking();
    
    UFUNCTION(BlueprintCallable)
    void FireOnPlayerTurning();
    
    UFUNCTION(BlueprintCallable)
    void FireOnPlayerStarted();
    
    UFUNCTION(BlueprintCallable)
    void FireOnPlayerSleep(const float Seconds, const bool bIsBlackout);
    
    UFUNCTION(BlueprintCallable)
    void FireOnPlayerRunning();
    
    UFUNCTION(BlueprintCallable)
    void FireOnPlayerReset();
    
    UFUNCTION(BlueprintCallable)
    void FireOnPlayerMoneyChanged(const int32 Value, const TArray<FString>& AnalyticsParams);
    
    UFUNCTION(BlueprintCallable)
    void FireOnPlayerLevelChanged(const int32 PlayerLevel);
    
    UFUNCTION(BlueprintCallable)
    void FireOnPlayerKilledNPC(AActor* NPCActor, FGameplayTag weaponTag);
    
    UFUNCTION(BlueprintCallable)
    void FireOnPlayerEyesOpen(bool bIsOpen);
    
    UFUNCTION(BlueprintCallable)
    void FireOnPlayerDied();
    
    UFUNCTION(BlueprintCallable)
    void FireOnPlayerBodyVisibilityChanged(const bool bValue);
    
    UFUNCTION(BlueprintCallable)
    void FireOnPictureTaken(AActor* Actor);
    
    UFUNCTION(BlueprintCallable)
    void FireOnObjectiveRestored(UMissionData* MissionData, UMissionAbstractObjectiveData* objectiveData);
    
    UFUNCTION(BlueprintCallable)
    void FireOnObjectiveProgress(UMissionData* MissionData, UMissionAbstractObjectiveData* objectiveData);
    
    UFUNCTION(BlueprintCallable)
    void FireOnObjectiveFinished(UMissionData* MissionData, UMissionAbstractObjectiveData* objectiveData);
    
    UFUNCTION(BlueprintCallable)
    void FireOnNPCDied(AActor* NPCActor);
    
    UFUNCTION(BlueprintCallable)
    void FireOnNewMissionAdded(UMissionData* MissionData);
    
    UFUNCTION(BlueprintCallable)
    void FireOnNewJournalNote(FGameplayTag NoteID);
    
    UFUNCTION(BlueprintCallable)
    void FireOnMissionStateChanged(UMissionData* MissionData);
    
    UFUNCTION(BlueprintCallable)
    void FireOnMissionPreview(UMissionData* MissionData);
    
    UFUNCTION(BlueprintCallable)
    void FireOnMissionFinished(UMissionData* MissionData);
    
    UFUNCTION(BlueprintCallable)
    void FireOnLevelStarted(FGameplayTag LevelTag);
    
    UFUNCTION(BlueprintCallable)
    void FireOnLevelPrepare(FGameplayTag LevelTag);
    
    UFUNCTION(BlueprintCallable)
    void FireOnLanguageChanged();
    
    UFUNCTION(BlueprintCallable)
    void FireOnItemUpdated(const FGameplayTag ItemTag, const FGameplayTag UpgradeID);
    
    UFUNCTION(BlueprintCallable)
    void FireOnItemTaken(UInventoryItem* InventoryItem);
    
    UFUNCTION(BlueprintCallable)
    void FireOnItemStored(UInventoryItem* InventoryItem);
    
    UFUNCTION(BlueprintCallable)
    void FireOnItemsSold(TArray<UInventoryItem*> Items);
    
    UFUNCTION(BlueprintCallable)
    void FireOnItemRemoved(UInventoryItem* InventoryItem);
    
    UFUNCTION(BlueprintCallable)
    void FireOnItemDropped(AActor* itemActor);
    
    UFUNCTION(BlueprintCallable)
    void FireOnItemChangeDurability(UInventoryItem* InventoryItem);
    
    UFUNCTION(BlueprintCallable)
    void FireOnItemBought(UInventoryItem* InventoryItem);
    
    UFUNCTION(BlueprintCallable)
    void FireOnItemActorRefRemoved(UInventoryItem* InventoryItem);
    
    UFUNCTION(BlueprintCallable)
    void FireOnInventoryModeChanged();
    
    UFUNCTION(BlueprintCallable)
    void FireOnHolderFixed(const FGameplayTag HolderID, const int32 TotalItems, const TArray<FString>& FixedItems);
    
    UFUNCTION(BlueprintCallable)
    void FireOnGameTimeChanged(float DeltaSeconds);
    
    UFUNCTION(BlueprintCallable)
    void FireOnGameStarted();
    
    UFUNCTION(BlueprintCallable)
    void FireOnGameShutdown();
    
    UFUNCTION(BlueprintCallable)
    void FireOnGameplayTimeDilationChanged(float TimeDilation);
    
    UFUNCTION(BlueprintCallable)
    void FireOnGamePaused(const bool bIsPaused);
    
    UFUNCTION(BlueprintCallable)
    void FireOnGameModeStarted();
    
    UFUNCTION(BlueprintCallable)
    void FireOnGameInstanceStarted();
    
    UFUNCTION(BlueprintCallable)
    void FireOnGameDataSaved(const int32 SlotNr, const int32 SaveNr, const float SizeMb);
    
    UFUNCTION(BlueprintCallable)
    void FireOnGameDataSave();
    
    UFUNCTION(BlueprintCallable)
    void FireOnFirstTimeTaken(const FGameplayTag ItemTag);
    
    UFUNCTION(BlueprintCallable)
    void FireOnEnvironmentRePrepared(const APREnvironment* Environment);
    
    UFUNCTION(BlueprintCallable)
    void FireOnDifficultyChanged();
    
    UFUNCTION(BlueprintCallable)
    void FireOnCustomMapMarkPassed(FGameplayTag MarkID);
    
    UFUNCTION(BlueprintCallable)
    void FireOnConsumableUsing(UInventoryItem* Item);
    
    UFUNCTION(BlueprintCallable)
    void FireOnConsumableActorUsing(AActor* itemActor);
    

    // Fix for true pure virtual functions not being implemented
};

