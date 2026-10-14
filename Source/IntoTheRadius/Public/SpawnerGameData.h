#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/NoExportTypes.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "AnomalyData.h"
#include "AnomalyFieldData.h"
#include "FurnitureData.h"
#include "IClearable.h"
#include "NPCDataMap.h"
#include "NPCGameData.h"
#include "NameArray.h"
#include "SpawnSlotData.h"
#include "SpawnerGameData.generated.h"

class APRAnomaly;
class UInventoryItem;

UCLASS(Blueprintable)
class INTOTHERADIUS_API USpawnerGameData : public UObject, public IIClearable {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<FString, FNPCDataMap> npcSpawners;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<FString, FAnomalyFieldData> anomalySpawners;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<FString, FAnomalyData> anomalies;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<FString, int32> SavedCorpseTypes;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<FString, FFurnitureData> FurnitureLootData;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<FString, FNameArray> FurnitureBrokenData;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<FString, FNameArray> FurnitureEmptySlots;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<FString, FSpawnSlotData> LootSpawnerData;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<FString, FNameArray> LootTableExludes;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<FString, int32> LootTablePoints;
    
    UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess=true))
    int64 spawnSeed;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FString> PickedUpItems;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FGameplayTag> SpecialClusters;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FGameplayTag> SpecialGenerators;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FGameplayTag> ExcludedGenerators;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<FString, float> GeneratorsHP;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<FString, FTransform> GeneratorsTransforms;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<FString, float> GeneratorsCooldown;
    
    USpawnerGameData();

    UFUNCTION(BlueprintCallable, BlueprintPure)
    FString ToString() const;
    
    UFUNCTION(BlueprintCallable)
    void SpawnSavedItems(const FString& SpawnerId);
    
    UFUNCTION(BlueprintCallable)
    void SpawnSavedArtifacts(const FString& SpawnerId);
    
    UFUNCTION(BlueprintCallable)
    void SpawnSavedAnomalies(const FString& SpawnerId);
    
    UFUNCTION(BlueprintCallable)
    void SpawnLootSpawnerData(const FString& InstanceId, bool& HasData, bool& NeedPlacement, TArray<UInventoryItem*>& spawned);
    
    UFUNCTION(BlueprintCallable)
    void SaveSpawnedLootData(const FString& InstanceId, const TArray<UInventoryItem*> Loot);
    
    UFUNCTION(BlueprintCallable)
    void SaveSpawnedItemData(const FString& SpawnerId, UInventoryItem* Item, const FTransform Transform);
    
    UFUNCTION(BlueprintCallable)
    void SaveSpawnedArtifactData(const FString& SpawnerId, const FString& InstanceId, const FGameplayTag TypeID, const FTransform Transform);
    
    UFUNCTION(BlueprintCallable)
    void SaveSpawnedAnomalyData(const FString& SpawnerId, APRAnomaly* anomaly);
    
    UFUNCTION(BlueprintCallable)
    void SaveNpcData(const FString& SpawnerId, const FNPCGameData Data);
    
    UFUNCTION(BlueprintCallable)
    void SaveGeneratorData(const FString& generatorID, const float HP, const FTransform Transform, const float Cooldown);
    
    UFUNCTION(BlueprintCallable)
    void SaveGeneratedLootData(const FString& InstanceId, const TArray<UInventoryItem*> Loot);
    
    UFUNCTION(BlueprintCallable)
    void SaveFurnitureSocketData(const FString& InstanceId, const TMap<FName, FTransform>& Data, const TArray<FName>& Broken);
    
    UFUNCTION(BlueprintCallable)
    void SaveFurnitureEmptySocket(const FString& InstanceId, const FName& EmptySlotId);
    
    UFUNCTION(BlueprintCallable)
    void SaveEmptyLootData(const FString& InstanceId);
    
    UFUNCTION(BlueprintCallable)
    void SaveBodyType(const FString& SpawnerId, const int32 BodyType);
    
    UFUNCTION(BlueprintCallable)
    void SaveAnomalyData(const FString& SpawnerId, const FAnomalyData Data);
    
    UFUNCTION(BlueprintCallable)
    void RemoveSpawnedItemData(UInventoryItem* Item);
    
    UFUNCTION(BlueprintCallable)
    void RemoveNpcData(const FString& SpawnerId, const FString& InstanceId);
    
    UFUNCTION(BlueprintCallable)
    void RemoveFieldDataArtifact(const FString& InstanceId);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FSpawnSlotData MakeFurnitureSlotData(TArray<UInventoryItem*> Items);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsGeneratorSaved(const FString& generatorID) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool HasSpawner(const FString& SpawnerId) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void HasLootSpawnerData(const FString& InstanceId, bool& bHasData) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FRandomStream GetRandomStream(const FString& SpawnerId) const;
    
    UFUNCTION(BlueprintCallable)
    void GetNpcData(const FString& SpawnerId, TMap<FString, FNPCGameData>& DataMap);
    
    UFUNCTION(BlueprintCallable)
    void GetLootSpawnerData(const FString& InstanceId, TArray<UInventoryItem*>& Loot, bool& bIsEmpty, bool& bHasData);
    
    UFUNCTION(BlueprintCallable)
    void GetFurnitureData(const FString& InstanceId, FFurnitureData& Data, FNameArray& BrokenSlots, FNameArray& EmptySlots, bool& bHasData);
    
    UFUNCTION(BlueprintCallable)
    int32 GetBodyType(const FString& SpawnerId);
    
    UFUNCTION(BlueprintCallable)
    void GetAnomalyFieldData(const FString& SpawnerId, FAnomalyFieldData& Data);
    
    UFUNCTION(BlueprintCallable)
    FAnomalyData GetAnomalyData(const FString& SpawnerId);
    

    // Fix for true pure virtual functions not being implemented
};

