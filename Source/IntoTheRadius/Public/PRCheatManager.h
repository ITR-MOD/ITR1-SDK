#pragma once
#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "GameplayTagContainer.h"
#include "PRCheatManager.generated.h"

class UInventoryItem;
class UMissionData;

UCLASS(Blueprintable)
class INTOTHERADIUS_API UPRCheatManager : public UCheatManager {
    GENERATED_BODY()
public:
    UPRCheatManager();

    UFUNCTION(BlueprintCallable, Exec)
    static void UnlockAllWeapons();
    
    UFUNCTION(BlueprintCallable, Exec)
    void ToggleProjectileDraw();
    
    UFUNCTION(BlueprintCallable, Exec)
    void SpawnItem(const FString& Tag);
    
    UFUNCTION(BlueprintCallable, Exec)
    static void ShowRevision();
    
    UFUNCTION(BlueprintCallable)
    void SetSkipTutorial(bool b_new_skip);
    
    UFUNCTION(BlueprintCallable, Exec)
    static void SecurityLevel(const int32 Level);
    
    UFUNCTION(BlueprintCallable, Exec)
    void RIP() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void ResetTideTimer() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    void RefillAmmoContainer(UInventoryItem* Item) const;
    
    UFUNCTION(BlueprintCallable, Exec)
    void RefillAmmo() const;
    
    UFUNCTION(BlueprintCallable)
    void RaiseCrashReport();
    
    UFUNCTION(BlueprintCallable, Exec)
    void PrintActors();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsProjectileDraw() const;
    
    UFUNCTION(BlueprintCallable, Exec)
    void GiveMeTheWeapon(const FString& Tag);
    
    UFUNCTION(BlueprintCallable, Exec)
    static void GiveMeTheMoney(const int32 Amount);
    
    UFUNCTION(BlueprintCallable, Exec)
    static void GiveMeTheDurability(const int32 Value);
    
    UFUNCTION(BlueprintCallable, Exec)
    void GiveMeTheBullets();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool GetSkipTutorial() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetMission(const FGameplayTag MissionInstanceId, UMissionData*& MissionData) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    TArray<FGameplayTag> GetChildTags(const FGameplayTag parentTag);
    
    UFUNCTION(BlueprintCallable)
    UInventoryItem* DoSpawnItem(const FGameplayTag Tag);
    
    UFUNCTION(BlueprintCallable, Exec)
    void ChangeHealth(const int32 Delta) const;
    
    UFUNCTION(BlueprintCallable)
    void AddMission(UMissionData* MissionData);
    
};

