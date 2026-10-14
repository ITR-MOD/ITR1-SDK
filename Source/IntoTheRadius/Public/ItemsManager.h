#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "IClearable.h"
#include "ItemsManager.generated.h"

class UInventoryItem;
class UItemsManager;
class UPRItemConfig;
class USceneComponent;

UCLASS(Blueprintable)
class INTOTHERADIUS_API UItemsManager : public UObject, public IIClearable {
    GENERATED_BODY()
public:
    UItemsManager();

    UFUNCTION(BlueprintCallable)
    void SetConditionsIgnore(bool ignoreCorpse, bool ignoreStash);
    
    UFUNCTION(BlueprintCallable)
    void PackItems(USceneComponent* Parent, TArray<UInventoryItem*> Items, TArray<UInventoryItem*>& Excluded, const bool bExcludeNonFit, const bool bProjectDown);
    
    UFUNCTION(BlueprintCallable)
    void MultiPackItems(TArray<USceneComponent*> Bounds, TArray<UInventoryItem*> Items, const bool bProjectDown);
    
    UFUNCTION(BlueprintCallable)
    void GetLootForTable(FGameplayTag lootTableTag, int32 LootPoints, const FString& idOverride, TArray<UInventoryItem*>& Items, bool& success);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static UItemsManager* GetItemsManager();
    
    UFUNCTION(BlueprintCallable)
    void GetDefaultLoot(TArray<FGameplayTag> itemTags, const FString& idOverride, TArray<UInventoryItem*>& Items);
    
    UFUNCTION(BlueprintCallable)
    void GetCurrentLevelLoot(const int32 LootPoints, const FString& idOverride, TArray<UInventoryItem*>& Items);
    
    UFUNCTION(BlueprintCallable)
    void GetConfiguredLoot(TArray<UPRItemConfig*> ItemConfigs, const FString& idOverride, TArray<UInventoryItem*>& Items);
    

    // Fix for true pure virtual functions not being implemented
};

