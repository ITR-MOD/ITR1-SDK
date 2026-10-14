#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "RadiusGameDifficulty.h"
#include "AnalyticsManager.generated.h"

class AActor;
class UMissionData;
class UPRGameInstance;

UCLASS(Blueprintable)
class INTOTHERADIUS_API UAnalyticsManager : public UObject {
    GENERATED_BODY()
public:
    UAnalyticsManager();

    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnUnstuck(FVector Location);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnTutorialStart(const FGameplayTag TutorialId, const int32 TotalSteps);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnTutorialEnd(const FGameplayTag TutorialId, const int32 TotalSteps, const int32 CompletedSteps);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnTutorialControls();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnTransitionStart();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnTideHappening();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnSleep(const float Seconds, const bool isBlackout);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnSettingsGameplay(bool VirtualStock, bool PlayerHeight, bool NeckHeight, bool ArmLength, bool VignetteTurn, bool VignetteRun, bool HighlightDroppedItems, bool ShowBody, bool SeatedMode);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnSettingsControls(const bool bTurnType, const bool bTurnSpeed, const bool bMovementDirectionType, const bool bControllerRotation, const bool bHoldGrip, const bool bLeftHanded, const bool bMirrorHolsters, const bool bTeleportMovement);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnPlayerLocation(FVector Location, float AverageDelta);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnPlayerKill(AActor* KilledActor, const FGameplayTag weaponTag, const bool bIsSmoothing, const FVector DeathLocation);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnPlayerDeath(const FGameplayTag DeathCauser, const FVector DeathLocation);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnMoneyChanged(const int32 Value, const TArray<FString>& AnalyticsParams);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnMissionStateChanged(UMissionData* MissionData);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnMissionFinished(UMissionData* MissionData);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnMenuNewGame(const bool SkipTutor, const bool Ironman, const FRadiusGameDifficulty Difficulty);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnLogHmdDevice();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnLogFPS(const FString& Location, float Min, float Max, float Average);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnLevelStarted(const FGameplayTag Value);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnKatyasMonologue(FGameplayTag MonologueTag);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnJournalOpened(const FGameplayTag TabTag);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnGuitarPlayed(const int32 PlayCount, const float PlayTimeSeconds);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnGameStart();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnFinalChoice(const FString& Choice);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnDifficultyChanged();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void End();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void Begin(UPRGameInstance* GameInstance);
    
};

