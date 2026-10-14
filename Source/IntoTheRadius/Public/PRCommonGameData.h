#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "GameplayTagContainer.h"
#include "ShootingScores.h"
#include "PRCommonGameData.generated.h"

class UInventoryItem;
class UMissionData;
class UPRCommonGameData;

UCLASS(Blueprintable)
class INTOTHERADIUS_API UPRCommonGameData : public UObject {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FDateTime SaveDateTime;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FShootingScores> ShootingScoreboard;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FShootingScores LastShootingScores;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FGameplayTagContainer ObtainedWeapons;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FGameplayTagContainer FinishedMissions;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    int32 FinishedMissionsCount;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FGameplayTag> CommonTriggers;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FString FirstSessionDateString;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    int32 MaxPlayerLevel;
    
    UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess=true))
    int64 GameTotalTicks;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    int32 SessionCount;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    int32 GuitarPlayCount;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool bCachedHasSupporterPack;
    
    UPRCommonGameData();

    UFUNCTION(BlueprintCallable)
    void SetTrigger(const FGameplayTag Tag);
    
    UFUNCTION(BlueprintCallable)
    void SaveShootingRun(const float RunTime, const int32 Scores, const int32 Hits, const int32 Misses, const int32 GrenadeKills);
    
private:
    UFUNCTION(BlueprintCallable)
    void OnMissionChange(UMissionData* MissionData);
    
    UFUNCTION(BlueprintCallable)
    void OnItemTaken(UInventoryItem* Item);
    
public:
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsWeaponUnlocked(const FGameplayTag Tag);
    
    UFUNCTION(BlueprintCallable)
    void Initialize();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    int32 IncrementGuitarPlayCount();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool GetTrigger(const FGameplayTag Tag);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    float GetTotalPlaytimeMinutes();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static void GetCommonGameData(UPRCommonGameData*& Data);
    
};

