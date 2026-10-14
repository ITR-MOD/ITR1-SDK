#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "AmmoInfo.h"
#include "AttachmentSlotInfo.h"
#include "IAsset.h"
#include "IPreparable.h"
#include "PRAmmoContainerInfo.h"
#include "PRAnomalyInfo.h"
#include "PRItemInfo.h"
#include "PRNPCInfo.h"
#include "Templates/SubclassOf.h"
#include "UpgradeInfo.h"
#include "WeaponInfo.h"
#include "InfosConfig.generated.h"

class AActor;
class APRAnomaly;
class UDataTable;
class UInfosConfig;

UCLASS(Blueprintable)
class INTOTHERADIUS_API UInfosConfig : public UDataAsset, public IIAsset, public IIPreparable {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    UDataTable* ItemInfosTable;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    UDataTable* AmmoContainerInfosTable;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    UDataTable* AmmoInfosTable;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    UDataTable* WeaponInfosTable;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    UDataTable* UpgradeInfosTable;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    UDataTable* NPCInfosTable;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    UDataTable* AnomalyInfosTable;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    UDataTable* AttachmentSlotInfosTable;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<FGameplayTag, FText> AdditionalNames;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<FGameplayTag, FText> AmmoCaliberNames;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<FGameplayTag, FText> AmmoTypeNames;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FGameplayTag> ExcludeFromSave;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<FGameplayTag, FGameplayTag> ReplaceItemTagsFromSave;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float MaxWeaponDamage;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float MaxWeaponShootingStability;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float MaxWeaponRecoilForce;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float MaxWeaponAccuracy;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float MaxWeaponFireRate;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float MaxDurability;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float MaxReliability;
    
    UInfosConfig();

    UFUNCTION(BlueprintCallable)
    void SetItemProxyClass(FGameplayTag ItemTypeID, UClass* ProxyClass);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FGameplayTag ResolveItemTypeID(const FGameplayTag ItemTypeID) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsValidNPC(FGameplayTag npcTypeID) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsValidItemTypeID(const FGameplayTag ItemTypeID) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsValidAnomaly(const FGameplayTag anomalyTypeID) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsAmmoContainer(const FGameplayTag ItemTypeID) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FUpgradeInfo GetUpgradeInfo(const FGameplayTag UpgradeID) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    int32 GetTypeSellPrice(const FGameplayTag ItemTypeID) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    int32 GetTypeLevelRequirement(const FGameplayTag ItemTypeID) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    int32 GetTypeBuyPrice(const FGameplayTag ItemTypeID) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FAttachmentSlotInfo GetSlotInfo(const FGameplayTag TypeID) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    TArray<FGameplayTag> GetNPCsListByCores(TArray<FGameplayTag> coreTags) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    TMap<FGameplayTag, FPRNPCInfo> GetNPCInfos() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FPRNPCInfo GetNPCInfo(FGameplayTag npcTypeID) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    TSubclassOf<AActor> GetNPCClass(FGameplayTag npcTypeID) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FGameplayTag GetNPCByCore(FGameplayTag coreTag) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    UClass* GetItemProxyClass(FGameplayTag ItemTypeID) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    TSubclassOf<AActor> GetItemClass(FGameplayTag ItemTypeID) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static UInfosConfig* GetInfosConfig();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    TArray<FPRAmmoContainerInfo> GetContainersForAmmo(FGameplayTag AmmoTypeID) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FWeaponInfo GetBasicWeaponInfo(const FGameplayTag ItemTypeID) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FPRItemInfo GetBasicItemInfo(FGameplayTag ItemTypeID) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FPRAmmoContainerInfo GetBasicAmmoContainerInfo(FGameplayTag ItemTypeID) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FGameplayTag GetAnotherItemTypeID(FGameplayTag ItemTypeID, bool prev) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FPRAnomalyInfo GetAnomalyInfo(const FGameplayTag anomalyTypeID) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    TSubclassOf<APRAnomaly> GetAnomalyClass(FGameplayTag anomalyTypeID) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FText GetAmmoTypeName(const FGameplayTag TypeID) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FAmmoInfo GetAmmoInfo(const FGameplayTag ItemTypeID) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FText GetAmmoCaliberName(const FGameplayTag CaliberID) const;
    

    // Fix for true pure virtual functions not being implemented
};

