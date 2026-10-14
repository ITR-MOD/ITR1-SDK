#include "InfosConfig.h"
#include "Templates/SubclassOf.h"

UInfosConfig::UInfosConfig() {
    this->ItemInfosTable = NULL;
    this->AmmoContainerInfosTable = NULL;
    this->AmmoInfosTable = NULL;
    this->WeaponInfosTable = NULL;
    this->UpgradeInfosTable = NULL;
    this->NPCInfosTable = NULL;
    this->AnomalyInfosTable = NULL;
    this->AttachmentSlotInfosTable = NULL;
    this->MaxWeaponDamage = 250.00f;
    this->MaxWeaponShootingStability = 200.00f;
    this->MaxWeaponRecoilForce = 200.00f;
    this->MaxWeaponAccuracy = 100.00f;
    this->MaxWeaponFireRate = 1500.00f;
    this->MaxDurability = 100.00f;
    this->MaxReliability = 3.00f;
}

void UInfosConfig::SetItemProxyClass(FGameplayTag ItemTypeID, UClass* ProxyClass) {
}

FGameplayTag UInfosConfig::ResolveItemTypeID(const FGameplayTag ItemTypeID) const {
    return FGameplayTag{};
}

bool UInfosConfig::IsValidNPC(FGameplayTag npcTypeID) const {
    return false;
}

bool UInfosConfig::IsValidItemTypeID(const FGameplayTag ItemTypeID) const {
    return false;
}

bool UInfosConfig::IsValidAnomaly(const FGameplayTag anomalyTypeID) const {
    return false;
}

bool UInfosConfig::IsAmmoContainer(const FGameplayTag ItemTypeID) const {
    return false;
}

FUpgradeInfo UInfosConfig::GetUpgradeInfo(const FGameplayTag UpgradeID) const {
    return FUpgradeInfo{};
}

int32 UInfosConfig::GetTypeSellPrice(const FGameplayTag ItemTypeID) const {
    return 0;
}

int32 UInfosConfig::GetTypeLevelRequirement(const FGameplayTag ItemTypeID) const {
    return 0;
}

int32 UInfosConfig::GetTypeBuyPrice(const FGameplayTag ItemTypeID) const {
    return 0;
}

FAttachmentSlotInfo UInfosConfig::GetSlotInfo(const FGameplayTag TypeID) const {
    return FAttachmentSlotInfo{};
}

TArray<FGameplayTag> UInfosConfig::GetNPCsListByCores(TArray<FGameplayTag> coreTags) const {
    return TArray<FGameplayTag>();
}

TMap<FGameplayTag, FPRNPCInfo> UInfosConfig::GetNPCInfos() const {
    return TMap<FGameplayTag, FPRNPCInfo>();
}

FPRNPCInfo UInfosConfig::GetNPCInfo(FGameplayTag npcTypeID) const {
    return FPRNPCInfo{};
}

TSubclassOf<AActor> UInfosConfig::GetNPCClass(FGameplayTag npcTypeID) const {
    return NULL;
}

FGameplayTag UInfosConfig::GetNPCByCore(FGameplayTag coreTag) const {
    return FGameplayTag{};
}

UClass* UInfosConfig::GetItemProxyClass(FGameplayTag ItemTypeID) const {
    return NULL;
}

TSubclassOf<AActor> UInfosConfig::GetItemClass(FGameplayTag ItemTypeID) const {
    return NULL;
}

UInfosConfig* UInfosConfig::GetInfosConfig() {
    return NULL;
}

TArray<FPRAmmoContainerInfo> UInfosConfig::GetContainersForAmmo(FGameplayTag AmmoTypeID) const {
    return TArray<FPRAmmoContainerInfo>();
}

FWeaponInfo UInfosConfig::GetBasicWeaponInfo(const FGameplayTag ItemTypeID) const {
    return FWeaponInfo{};
}

FPRItemInfo UInfosConfig::GetBasicItemInfo(FGameplayTag ItemTypeID) const {
    return FPRItemInfo{};
}

FPRAmmoContainerInfo UInfosConfig::GetBasicAmmoContainerInfo(FGameplayTag ItemTypeID) const {
    return FPRAmmoContainerInfo{};
}

FGameplayTag UInfosConfig::GetAnotherItemTypeID(FGameplayTag ItemTypeID, bool prev) const {
    return FGameplayTag{};
}

FPRAnomalyInfo UInfosConfig::GetAnomalyInfo(const FGameplayTag anomalyTypeID) const {
    return FPRAnomalyInfo{};
}

TSubclassOf<APRAnomaly> UInfosConfig::GetAnomalyClass(FGameplayTag anomalyTypeID) const {
    return NULL;
}

FText UInfosConfig::GetAmmoTypeName(const FGameplayTag TypeID) const {
    return FText::GetEmpty();
}

FAmmoInfo UInfosConfig::GetAmmoInfo(const FGameplayTag ItemTypeID) const {
    return FAmmoInfo{};
}

FText UInfosConfig::GetAmmoCaliberName(const FGameplayTag CaliberID) const {
    return FText::GetEmpty();
}


