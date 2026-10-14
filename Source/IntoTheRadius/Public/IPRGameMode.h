#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/NoExportTypes.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "NPCGameData.h"
#include "Templates/SubclassOf.h"
#include "IPRGameMode.generated.h"

class AActor;
class APRAnomaly;
class APREnvironment;
class UInventoryItem;
class UPRItemConfig;

UINTERFACE(Blueprintable)
class UIPRGameMode : public UInterface {
    GENERATED_BODY()
};

class IIPRGameMode : public IInterface {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool TransferToLevel(const FGameplayTag LevelTag, const FGameplayTag LocationTag);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    AActor* SpawnSavedNPC(const FNPCGameData Data, AActor* SpawnerActor, const float Radius, const bool bCanWalkAway, const bool SnipingMode);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    AActor* SpawnNPC(const FGameplayTag npcTypeID, AActor* SpawnerActor, const float Radius, const bool bCanWalkAway, const bool SnipingMode);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    AActor* SpawnItemConfig(UPRItemConfig* itemConfig, const FTransform Transform, const bool bProjectOnGround, const bool bSimulate, const bool bAddToPlayer);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    AActor* SpawnItemByType(const FGameplayTag& ItemTypeID, const FString& InstanceId, const FTransform& Transform, bool bProjectOnGround, bool bSimulate);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    AActor* SpawnItem(UInventoryItem* InventoryItem, FTransform Transform, bool bProjectOnGround, bool bSimulate);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    APRAnomaly* SpawnAnomaly(const FString& AnomalyName, TSubclassOf<APRAnomaly> AnomalyClass, FTransform AnomalyTransform, bool bProjectOnGround, float AnomalyRadius);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool SleepAllowed() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void SetSleepAllowed(const bool bAllow);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void ResetPlayerAndItems();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    FVector ReprojectLocation(FVector Location);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void ReEnterCurrentLevel();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnPlayerDeath();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void LaunchMainMenu();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void LaunchGame(const bool bStart);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    float GetNoiseAwarenessValue(FName Tag) const;
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    APREnvironment* GetEnvironment();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    float GetDamageAwarenessValue(float Damage) const;
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    FTransform GetCurrentPlayerTransform() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void FinishTutorial();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void FindSpawnLocation(FVector nearby, float InRadius, bool bReachable, bool& bSuccess, FVector& Location);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void ContinueAsFreeGame();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void ApplySoundVolumes();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void ApplyBrigtnessMult();
    
};

