#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/Object.h"
#include "UObject/NoExportTypes.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "EJournalGroup.h"
#include "EPRGameDifficultyPreset.h"
#include "GameDifficulty.h"
#include "IClearable.h"
#include "IPreparable.h"
#include "LevelDecorData.h"
#include "MapData.h"
#include "NoteData.h"
#include "PlayerStats.h"
#include "RadiusGameDifficulty.h"
#include "PRGameData.generated.h"

class UGameInventory;
class UGameMissions;
class UInventoryItem;
class UMissionAbstractObjectiveData;
class UMissionData;
class UPRGameData;
class UPlayerGameData;
class USpawnerGameData;
class UTextureRenderTarget2D;

UCLASS(Blueprintable)
class INTOTHERADIUS_API UPRGameData : public UObject, public IIClearable, public IIPreparable {
    GENERATED_BODY()
public:
private:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    UPlayerGameData* BackupPlayerData;
    
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    UPlayerGameData* PlayerData;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    UGameMissions* MissionsData;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    UGameInventory* InventoryData;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    USpawnerGameData* SpawnSaveData;
    
    UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess=true))
    uint64 UID;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    int32 SlotNr;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FDateTime SaveDateDime;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FString SaveMessage;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FPlayerStats PlayerStats;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FGameplayTag> VisitedLevels;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FDateTime TimeStart;
    
    UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess=true))
    int64 TicksPassed;
    
    UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess=true))
    int64 LastTideTicks;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    EPRGameDifficultyPreset Difficulty;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FGameDifficulty GameDifficulty;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FRadiusGameDifficulty GameDifficultyConfig;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool Ironman;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool IntactDifficulty;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FString GameUID;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FString GameStartDateString;
    
    UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess=true))
    int64 GameRealTicks;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<FGameplayTag, FMapData> MapsData;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<FGameplayTag, FLevelDecorData> LevelDecorItems;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FGameplayTag PreviousLevelTag;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FGameplayTag CurrentLevelTag;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FGameplayTag NextLevelTag;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FGameplayTag> PassedTriggers;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FString> CustomMarkers;
    
    UPRGameData();

    UFUNCTION(BlueprintCallable)
    void UpdateMarker(const FString& Marker, const bool bRemove);
    
    UFUNCTION(BlueprintCallable)
    bool UnsetTrigger(const FGameplayTag& Trigger);
    
    UFUNCTION(BlueprintCallable)
    bool StatAddUniqueItem(const FGameplayTag ItemId);
    
    UFUNCTION(BlueprintCallable)
    void StatAddKill(const FGameplayTag NPCTag, const FGameplayTag weaponTag);
    
    UFUNCTION(BlueprintCallable)
    void StatAddDeath(const FGameplayTag KillerTag);
    
    UFUNCTION(BlueprintCallable)
    bool SetTrigger(const FGameplayTag& Trigger);
    
    UFUNCTION(BlueprintCallable)
    void SetPlayerLevel(const int32 PlayerLevel);
    
    UFUNCTION(BlueprintCallable)
    void SaveSleepLocation(const FGameplayTag Level, const FVector Location);
    
    UFUNCTION(BlueprintCallable)
    void SaveDecorItemTransform(const FString& DecorId, const FTransform Transform);
    
    UFUNCTION(BlueprintCallable)
    void SaveDecorItemDestroyed(const FString& DecorId);
    
    UFUNCTION(BlueprintCallable)
    void SaveDeathLocation(const FGameplayTag Level, const FVector Location);
    
    UFUNCTION(BlueprintCallable)
    void ResetGameData();
    
    UFUNCTION(BlueprintCallable)
    void RemoveSleepLocation(const FGameplayTag Level);
    
    UFUNCTION(BlueprintCallable)
    void RemoveDeathLocation(const FGameplayTag Level);
    
    UFUNCTION(BlueprintCallable)
    void PrintMissions(const FString& Message);
    
    UFUNCTION(BlueprintCallable)
    void PrintInventory(const FString& Message);
    
    UFUNCTION(BlueprintCallable)
    void PrintData(const FString& Message);
    
    UFUNCTION(BlueprintCallable)
    void PrepareNewGame(const FRadiusGameDifficulty NewDifficulty, const bool bIronman, const bool bSkipTutorial, const bool bStartupConfig, const TArray<FGameplayTag> Triggers);
    
private:
    UFUNCTION(BlueprintCallable)
    void OnPlayerLevelChanged(int32 PlayerLevel);
    
    UFUNCTION(BlueprintCallable)
    void OnObjectiveChange(UMissionData* MissionData, UMissionAbstractObjectiveData* Objective);
    
    UFUNCTION(BlueprintCallable)
    void OnMissionChange(UMissionData* MissionData);
    
    UFUNCTION(BlueprintCallable)
    void OnItemTaken(UInventoryItem* Item);
    
    UFUNCTION(BlueprintCallable)
    void OnItemStored(UInventoryItem* Item);
    
public:
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsManOfHisWord() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsKatyaMonologueInvalid(const FGameplayTag& Tag, const bool bIgnoreConditions);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsJournalGroupHasNewNotes(EJournalGroup Group) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool HasMoney(const int32 Amount) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FString GetUID();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static void GetSpawnSaveData(USpawnerGameData*& NewSpawnSaveData);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool GetSleepLocation(const FGameplayTag Level, FVector& Location) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    float GetSecondsPassed() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    float GetSecondsLastTide() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    float GetRealSecondsToTide();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    float GetPlaytimeSeconds() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    float GetPlaytimeMinutes() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    int32 GetPlayerLevel() const;
    
    UFUNCTION(BlueprintCallable)
    void GetMapRenderTarget(const FGameplayTag LevelTag, UTextureRenderTarget2D*& RenderTarget);
    
    UFUNCTION(BlueprintCallable)
    TArray<FNoteData> GetJournalGroupNotes(EJournalGroup Group);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    float GetGameSecondsToTide();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static void GetGameData(UPRGameData*& GameData);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    EPRGameDifficultyPreset GetDifficultyPreset() const;
    
    UFUNCTION(BlueprintCallable)
    void GetDecorItemTransform(const FString& DecorId, FTransform& Transform, bool& bDestroyed, bool& bSuccess);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool GetDeathLocation(const FGameplayTag Level, FVector& Location) const;
    
    UFUNCTION(BlueprintCallable)
    void FillFrom(UPRGameData* Other);
    
    UFUNCTION(BlueprintCallable)
    void DumpData(const FString& Message);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool CheckTrigger(const FGameplayTag& Trigger);
    
    UFUNCTION(BlueprintCallable)
    void CheckTideTriggers();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool CheckMarker(const FString& Marker);
    
    UFUNCTION(BlueprintCallable)
    void CheckGameTriggers();
    
    UFUNCTION(BlueprintCallable)
    void ChangeMoney(const int32 ChangeBy, const TArray<FString>& AnalyticsParams);
    
    UFUNCTION(BlueprintCallable)
    bool ChangeGameDifficulty(const FRadiusGameDifficulty Value);
    
    UFUNCTION(BlueprintCallable)
    void ApplyTideReset();
    

    // Fix for true pure virtual functions not being implemented
};

