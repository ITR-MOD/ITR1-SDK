#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "AmmoContainerData.h"
#include "EDamageCategory.h"
#include "EItemSlot.h"
#include "EWeaponBoltState.h"
#include "EWeaponFireMode.h"
#include "NoParamsDelegateDelegate.h"
#include "PRAmmoContainerInfo.h"
#include "PRItemInfo.h"
#include "WeaponInfo.h"
#include "InventoryItem.generated.h"

class AActor;
class UInventoryItem;

UCLASS(Blueprintable)
class INTOTHERADIUS_API UInventoryItem : public UDataAsset {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FNoParamsDelegate OnChangeDurability;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FString InstanceId;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FGameplayTag ItemId;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FString AttachedTo;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    EItemSlot AttachmentSlot;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FGameplayTag HolderID;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FTransform HolderRelativeTransform;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float Durability;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FAmmoContainerData AmmoInfo;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FString> Attachments;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    AActor* ActorReference;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FGameplayTag> AdditionalTags;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FGameplayTag> Upgrades;
    
    UInventoryItem();

    UFUNCTION(BlueprintCallable)
    bool UpdateUpgrades(const TArray<FGameplayTag> UpgradeIDs, const bool bChangeMoney);
    
    UFUNCTION(BlueprintCallable)
    void UpdateDurabilityByUpgrade(const FGameplayTag UpgradeID);
    
    UFUNCTION(BlueprintCallable)
    bool SwitchFireMode();
    
    UFUNCTION(BlueprintCallable)
    bool SetFireMode(const EWeaponFireMode FireMode);
    
    UFUNCTION(BlueprintCallable)
    void SetDurability(const float Value, bool& bSuccess);
    
    UFUNCTION(BlueprintCallable)
    void SetChamberAmmo(const FGameplayTag ChamberAmmo);
    
    UFUNCTION(BlueprintCallable)
    void SetBoltState(const EWeaponBoltState BoltState);
    
    UFUNCTION(BlueprintCallable)
    void SetAmmo(TArray<FGameplayTag> Ammo);
    
    UFUNCTION(BlueprintCallable)
    bool RemoveUpgrade(const FGameplayTag UpgradeID);
    
    UFUNCTION(BlueprintCallable)
    void RemoveExactAmmo(int32 Index, bool& bSuccess, FGameplayTag& Ammo);
    
    UFUNCTION(BlueprintCallable)
    void RemoveAmmo(bool& bSuccess, FGameplayTag& Ammo);
    
    UFUNCTION(BlueprintCallable)
    void PrintItem(const FString& Message);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsUpgradeAvailable(const FGameplayTag UpgradeID, const bool bMoneyCheck);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsRepairable();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsArmorItem();
    
    UFUNCTION(BlueprintCallable)
    bool IsAmmoTypeAcceptable(FGameplayTag AmmoTag);
    
    UFUNCTION(BlueprintCallable)
    void IsActorOfAcceptableAmmo(AActor* AmmoActor, bool& bAcceptable, FGameplayTag& AmmoTag);
    
    UFUNCTION(BlueprintCallable)
    bool InstallUpgrade(const FGameplayTag UpgradeID, const bool bChangeMoney);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool HasFireMode(const EWeaponFireMode FireMode);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool HasDamageCategoryArmor(EDamageCategory DamageCategory);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FWeaponInfo GetWeaponInfo();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetSilencerAttachment(UInventoryItem*& AttachItem);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    int32 GetSellPrice(bool bIgnoreIfAttached, bool bItemOnly);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    int32 GetRepairTime(bool bIgnoreIfAttached);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    int32 GetRepairPrice(bool bIgnoreIfAttached);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    TArray<FGameplayTag> GetPossibleUpgrades();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetNextAmmo(FGameplayTag& NextAmmoTag);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    float GetMaxDurability();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    int32 GetMaxAmmoCapacity();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetMagAttachment(UInventoryItem*& AttachItem);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    float GetItemWeight(bool bIgnoreAttachments);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FPRItemInfo GetItemInfo();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    EWeaponFireMode GetFireMode() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    float GetDurabilityRatio();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    float GetDamageCategoryArmor(EDamageCategory DamageCategory);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    EWeaponBoltState GetBoltState() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetAttachments(const FGameplayTag TypeTag, TArray<UInventoryItem*>& AttachItems);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FString GetAmmoTypeName();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FString GetAmmoString();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FPRAmmoContainerInfo GetAmmoContainerInfo();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetAllAttachments(TArray<UInventoryItem*>& AttachItems);
    
    UFUNCTION(BlueprintCallable)
    void ChangeDurabilityByRatio(const float Ratio, const EDamageCategory DamageCategory, bool& bSuccess, float& ChangedBy);
    
    UFUNCTION(BlueprintCallable)
    void ChangeDurability(const float bY, const EDamageCategory DamageCategory, bool& bSuccess, float& ChangedBy);
    
    UFUNCTION(BlueprintCallable)
    void ChangeAmmoContainerDurability(bool& bSuccess, float& ChangedBy);
    
    UFUNCTION(BlueprintCallable)
    void ChamberNextAmmo();
    
    UFUNCTION(BlueprintCallable)
    void AddAmmo(FGameplayTag AmmoTag, bool& bSuccess);
    
};

