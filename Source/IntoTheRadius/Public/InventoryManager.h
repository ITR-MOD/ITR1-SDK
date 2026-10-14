#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/NoExportTypes.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "EItemSlot.h"
#include "IClearable.h"
#include "IPreparable.h"
#include "InventoryManager.generated.h"

class AActor;
class UInventoryItem;
class UInventoryItemsCollection;
class UInventoryManager;
class UPRItemConfig;

UCLASS(Blueprintable)
class INTOTHERADIUS_API UInventoryManager : public UObject, public IIPreparable, public IIClearable {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool LevelInventoryEnabled;
    
    UInventoryManager();

    UFUNCTION(BlueprintCallable)
    void TakeActor(AActor* Actor, const FGameplayTag HolderID, bool& bSuccess, UInventoryItem*& InventoryItem);
    
    UFUNCTION(BlueprintCallable)
    void SellItems(TArray<AActor*> Items, float PriceMultiplier);
    
    UFUNCTION(BlueprintCallable)
    void RepairItems(TArray<UInventoryItem*> Items, bool& bSuccess);
    
    UFUNCTION(BlueprintCallable)
    void RemovePlayerItems(bool bDestroyActors);
    
    UFUNCTION(BlueprintCallable)
    void RemoveItemByRef(AActor* Actor, bool& success);
    
    UFUNCTION(BlueprintCallable)
    bool RemoveItem(UInventoryItem* Item);
    
    UFUNCTION(BlueprintCallable)
    void RecordUniqueItemSpawn(const FString& InstanceId, bool& success);
    
    UFUNCTION(BlueprintCallable)
    void RecordStashOpen(FGameplayTag stashType, const FString& stashID, bool& success);
    
private:
    UFUNCTION(BlueprintCallable)
    void OnSecondsAddedToTime(float Seconds);
    
public:
    UFUNCTION(BlueprintCallable)
    void KillPlayerItems();
    
    UFUNCTION(BlueprintCallable)
    void KillAllInventoryItems(bool fullClear);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void IsUniqueItemSpawned(const FString& InstanceId, bool& spawned);
    
    UFUNCTION(BlueprintCallable)
    void IsStashOpened(FGameplayTag stashType, const FString& stashID, bool& opened);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsNeedRepair(UInventoryItem* Item);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsInPlayerInventory(UInventoryItem* Item);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsInLevelsInventory(UInventoryItem* Item);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsInHubInventory(UInventoryItem* Item);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsActorInPlayerInventory(AActor* itemActor);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetSilencerAttachment(UInventoryItem* parentItem, UInventoryItem*& AttachItem);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetRepairStatus(bool& bEmpty, bool& bReady, TArray<UInventoryItem*>& NewRepairItems);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetPlayerItems(TArray<UInventoryItem*>& inventoryItems);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    UInventoryItemsCollection* GetPlayerInventoryCollection() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetMagAttachment(UInventoryItem* parentItem, UInventoryItem*& AttachItem);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetLevelsItems(TMap<FGameplayTag, UInventoryItemsCollection*>& levelsItems);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FGameplayTag GetItemLevel(UInventoryItem* Item);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetItemBP(const FString& InstanceId, UInventoryItem*& InventoryItem);
    
    UFUNCTION(BlueprintCallable)
    void GetInventoryWeight(const FGameplayTag inHolderID, float& TotalWeight);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static UInventoryManager* GetInventoryManager();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetCurrentLevelItems(TArray<UInventoryItem*>& inventoryItems);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetAttachments(UInventoryItem* parentItem, const FGameplayTag TypeTag, TArray<UInventoryItem*>& NewAttachItems);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetAttachedToSlot(UInventoryItem* parentItem, const EItemSlot Slot, UInventoryItem*& AttachItem);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetAllAttachments(UInventoryItem* parentItem, TArray<UInventoryItem*>& NewAttachItems);
    
    UFUNCTION(BlueprintCallable)
    void FindItemsByTagInPlayer(FGameplayTag Tag, bool strict, bool& success, TArray<UInventoryItem*>& inventoryItems);
    
    UFUNCTION(BlueprintCallable)
    void FindItemsByTagInLevel(FGameplayTag Level, FGameplayTag Tag, bool strict, bool& success, TArray<UInventoryItem*>& inventoryItems);
    
    UFUNCTION(BlueprintCallable)
    void FindItemsByTag(FGameplayTag Tag, bool strict, bool& success, TArray<UInventoryItem*>& inventoryItems);
    
    UFUNCTION(BlueprintCallable)
    void FindItemsByHolderID(FGameplayTag HolderID, bool& success, TArray<UInventoryItem*>& inventoryItems);
    
    UFUNCTION(BlueprintCallable)
    void FindItemByRefInPlayer(AActor* Actor, bool& success, UInventoryItem*& InventoryItem);
    
    UFUNCTION(BlueprintCallable)
    void FindItemByRef(AActor* Actor, bool& success, UInventoryItem*& InventoryItem);
    
    UFUNCTION(BlueprintCallable)
    void DropPlayerInventory(const bool bKillActors, const bool bDropOnGround, const FGameplayTag FromHolderID, const FVector LocationOffset);
    
    UFUNCTION(BlueprintCallable)
    void DropItemActor(AActor* Actor, bool bForceBackpackDrop, bool& bSuccess, UInventoryItem*& InventoryItem);
    
    UFUNCTION(BlueprintCallable)
    void DetachItems(UInventoryItem* itemParent, UInventoryItem* itemAttach, bool& success);
    
    UFUNCTION(BlueprintCallable)
    void DetachItemActors(AActor* Parent, AActor* attach, bool& success);
    
    UFUNCTION(BlueprintCallable)
    void ClearAllItemReferences();
    
    UFUNCTION(BlueprintCallable)
    void CheckPlayerItem(FGameplayTag ItemType, bool strictItemType, FGameplayTag HolderID, bool strictHolderID, bool& success);
    
    UFUNCTION(BlueprintCallable)
    bool CheckItem(const FString& itemInstanceID, bool pick, FGameplayTag HolderID, bool strictHolderID);
    
    UFUNCTION(BlueprintCallable)
    void CheckAttachments();
    
    UFUNCTION(BlueprintCallable)
    void BreakActorLink(AActor* Actor);
    
    UFUNCTION(BlueprintCallable)
    void AttachItems(UInventoryItem* itemParent, UInventoryItem* itemAttach, EItemSlot attachSlot, bool& success);
    
    UFUNCTION(BlueprintCallable)
    void AttachItemActors(AActor* Parent, AActor* attach, EItemSlot attachToSlot, bool& success);
    
    UFUNCTION(BlueprintCallable)
    void AddItemToPlayer(UInventoryItem* Item, bool& success);
    
    UFUNCTION(BlueprintCallable)
    void AddItemToLevel(UInventoryItem* Item, bool noAttachmentsCheck, bool& success);
    
    UFUNCTION(BlueprintCallable)
    void AddDefaultItemToLevel(FGameplayTag ItemTag, FTransform Transform, bool noAttachmentsCheck, bool& success);
    
    UFUNCTION(BlueprintCallable)
    UInventoryItem* AddConfigItemToLevel(UPRItemConfig* itemConfig, FTransform Transform, bool noAttachmentsCheck, bool& success);
    

    // Fix for true pure virtual functions not being implemented
};

