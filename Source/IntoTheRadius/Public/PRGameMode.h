#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/NoExportTypes.h"
#include "GameFramework/GameModeBase.h"
#include "GameplayTagContainer.h"
#include "DebugAndCheats.h"
#include "EPRGameModeState.h"
#include "IPRGameMode.h"
#include "IPRGameModeNative.h"
#include "ReadyMarkInterface.h"
#include "PRGameMode.generated.h"

class AActor;
class APREnvironment;
class APRGameMode;
class APawn;
class UMissionData;

UCLASS(Blueprintable, NonTransient)
class INTOTHERADIUS_API APRGameMode : public AGameModeBase, public IIPRGameMode, public IIPRGameModeNative, public IReadyMarkInterface, public IDebugAndCheats {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    EPRGameModeState State;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FTransform TransitionStartTransform;
    
protected:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool bRespawnInventory;
    
private:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    APawn* RadiusPlayerPawn;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool bSleepAllowed;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool bForcePause;
    
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool Autosave;
    
    APRGameMode(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable, Exec)
    void TogglePause();
    
protected:
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void TickDebug();
    
public:
    UFUNCTION(BlueprintCallable)
    void SetGameplayTimeDilation(const float Dilation);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void RestorePlayerIfDead();
    
private:
    UFUNCTION(BlueprintCallable)
    void ResetAutosaveTimer_Native();
    
    UFUNCTION(BlueprintCallable)
    void RecallLevelLoaded();
    
public:
    UFUNCTION(BlueprintCallable)
    void PreparePlayerForFinalChoice();
    
protected:
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void PrepareMissionExecutor();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnTransitionStart();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnTransitionHint();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnTransitionEnd();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnTideHappening(bool bReload);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnStartedGame();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnStartedEditor();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnMissionStateChanged(UMissionData* MissionData);
    
private:
    UFUNCTION(BlueprintCallable)
    void OnLevelUnloaded();
    
protected:
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnLevelStarted(FGameplayTag LevelTag);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnLevelPrepare(FGameplayTag LevelTag);
    
private:
    UFUNCTION(BlueprintCallable)
    void OnLevelLoaded();
    
protected:
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnInitializeGame();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnInitializeEditor();
    
private:
    UFUNCTION(BlueprintCallable)
    void OnGameStarted();
    
protected:
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnGameDataPreSave();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnEnteredMenu();
    
    UFUNCTION(BlueprintCallable)
    void OnEditorLevelLoaded();
    
public:
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FString MakeActorUID(AActor* Actor);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void KillActorsOnReload();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static void GetPRGameMode(APRGameMode*& GameMode);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetGameplayTimeDilation(float& Dilation) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    APREnvironment* GetCurrentEnvironment();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FString GetActorUID(AActor* Actor);
    
private:
    UFUNCTION(BlueprintCallable)
    void FinishTransfer();
    
protected:
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void FindLevelStart(const FGameplayTag LevelTag, const FGameplayTag LocationTag, FTransform& Transform, FVector& Velocity, bool& bFromSave);
    
private:
    UFUNCTION(BlueprintCallable)
    void DoTransferToLevel();
    
    UFUNCTION(BlueprintCallable)
    void DoStart();
    
    UFUNCTION(BlueprintCallable)
    void DoPrepare();
    
    UFUNCTION(BlueprintCallable)
    void DoEndTransition();
    
protected:
    UFUNCTION(BlueprintCallable)
    void ContinueTransitionLoad();
    
private:
    UFUNCTION(BlueprintCallable)
    void BeginWaitingForPSO();
    

    // Fix for true pure virtual functions not being implemented
};

