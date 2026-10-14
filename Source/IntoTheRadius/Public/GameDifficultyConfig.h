#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EDifficultyType.h"
#include "EItemsDropType.h"
#include "EPRGameDifficultyPreset.h"
#include "ETracersShowType.h"
#include "EnemySenseMultiplier.h"
#include "IAsset.h"
#include "RadiusGameDifficulty.h"
#include "GameDifficultyConfig.generated.h"

class UGameDifficultyConfig;

UCLASS(Blueprintable)
class INTOTHERADIUS_API UGameDifficultyConfig : public UDataAsset, public IIAsset {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<EPRGameDifficultyPreset, FRadiusGameDifficulty> Presets;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<EPRGameDifficultyPreset, FText> DifficultyPresetTypeNames;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<EDifficultyType, FText> DifficultyTypeNames;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<EItemsDropType, FText> ItemsDropTypeNames;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<ETracersShowType, FText> TracersShowTypeNames;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<EDifficultyType, FEnemySenseMultiplier> EnemySense;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<EDifficultyType, float> EnemyHealth;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<EDifficultyType, float> EnemyDamage;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<EDifficultyType, int32> EnemyCount;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<EDifficultyType, float> ItemSellPrice;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<EDifficultyType, float> WeaponShotDamage;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<EDifficultyType, float> AnomalyDamage;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<EDifficultyType, int32> AnomalyAmount;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<EDifficultyType, float> MissionMoneyReward;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<EDifficultyType, float> Hunger;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<EDifficultyType, float> TideTime;
    
    UGameDifficultyConfig();

    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsSleepRestoreHealth() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsShowTips() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsShowLocationOnMap() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsNoEnemies() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsNoAnomalies() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    float GetWeaponShotDamageMultiplier();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    ETracersShowType GetTracersShowType() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    float GetTideTimeMultiplier();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    float GetMissionMoneyRewardMultiplier();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    float GetItemSellPriceMultiplier();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    EItemsDropType GetItemsDropType();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    float GetHungerMultiplier();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static UGameDifficultyConfig* GetGameDifficultyConfig();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FEnemySenseMultiplier GetEnemySense();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    float GetEnemyHealthMultiplier();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    float GetEnemyDamageMultiplier();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    int32 GetEnemyCountModifier();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    float GetAnomalyDamageMultiplier();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    int32 GetAnomalyAmountModifier();
    

    // Fix for true pure virtual functions not being implemented
};

