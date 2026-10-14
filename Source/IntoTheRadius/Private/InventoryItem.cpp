#include "InventoryItem.h"

UInventoryItem::UInventoryItem() {
    this->AttachmentSlot = EItemSlot::None;
    this->Durability = 0.00f;
    this->ActorReference = NULL;
}

bool UInventoryItem::UpdateUpgrades(const TArray<FGameplayTag> UpgradeIDs, const bool bChangeMoney) {
    return false;
}

void UInventoryItem::UpdateDurabilityByUpgrade(const FGameplayTag UpgradeID) {
}

bool UInventoryItem::SwitchFireMode() {
    return false;
}

bool UInventoryItem::SetFireMode(const EWeaponFireMode FireMode) {
    return false;
}

void UInventoryItem::SetDurability(const float Value, bool& bSuccess) {
}

void UInventoryItem::SetChamberAmmo(const FGameplayTag ChamberAmmo) {
}

void UInventoryItem::SetBoltState(const EWeaponBoltState BoltState) {
}

void UInventoryItem::SetAmmo(TArray<FGameplayTag> Ammo) {
}

bool UInventoryItem::RemoveUpgrade(const FGameplayTag UpgradeID) {
    return false;
}

void UInventoryItem::RemoveExactAmmo(int32 Index, bool& bSuccess, FGameplayTag& Ammo) {
}

void UInventoryItem::RemoveAmmo(bool& bSuccess, FGameplayTag& Ammo) {
}

void UInventoryItem::PrintItem(const FString& Message) {
}

bool UInventoryItem::IsUpgradeAvailable(const FGameplayTag UpgradeID, const bool bMoneyCheck) {
    return false;
}

bool UInventoryItem::IsRepairable() {
    return false;
}

bool UInventoryItem::IsArmorItem() {
    return false;
}

bool UInventoryItem::IsAmmoTypeAcceptable(FGameplayTag AmmoTag) {
    return false;
}

void UInventoryItem::IsActorOfAcceptableAmmo(AActor* AmmoActor, bool& bAcceptable, FGameplayTag& AmmoTag) {
}

bool UInventoryItem::InstallUpgrade(const FGameplayTag UpgradeID, const bool bChangeMoney) {
    return false;
}

bool UInventoryItem::HasFireMode(const EWeaponFireMode FireMode) {
    return false;
}

bool UInventoryItem::HasDamageCategoryArmor(EDamageCategory DamageCategory) {
    return false;
}

FWeaponInfo UInventoryItem::GetWeaponInfo() {
    return FWeaponInfo{};
}

void UInventoryItem::GetSilencerAttachment(UInventoryItem*& AttachItem) {
}

int32 UInventoryItem::GetSellPrice(bool bIgnoreIfAttached, bool bItemOnly) {
    return 0;
}

int32 UInventoryItem::GetRepairTime(bool bIgnoreIfAttached) {
    return 0;
}

int32 UInventoryItem::GetRepairPrice(bool bIgnoreIfAttached) {
    return 0;
}

TArray<FGameplayTag> UInventoryItem::GetPossibleUpgrades() {
    return TArray<FGameplayTag>();
}

void UInventoryItem::GetNextAmmo(FGameplayTag& NextAmmoTag) {
}

float UInventoryItem::GetMaxDurability() {
    return 0.0f;
}

int32 UInventoryItem::GetMaxAmmoCapacity() {
    return 0;
}

void UInventoryItem::GetMagAttachment(UInventoryItem*& AttachItem) {
}

float UInventoryItem::GetItemWeight(bool bIgnoreAttachments) {
    return 0.0f;
}

FPRItemInfo UInventoryItem::GetItemInfo() {
    return FPRItemInfo{};
}

EWeaponFireMode UInventoryItem::GetFireMode() const {
    return EWeaponFireMode::Safety;
}

float UInventoryItem::GetDurabilityRatio() {
    return 0.0f;
}

float UInventoryItem::GetDamageCategoryArmor(EDamageCategory DamageCategory) {
    return 0.0f;
}

EWeaponBoltState UInventoryItem::GetBoltState() const {
    return EWeaponBoltState::Closed;
}

void UInventoryItem::GetAttachments(const FGameplayTag TypeTag, TArray<UInventoryItem*>& AttachItems) {
}

FString UInventoryItem::GetAmmoTypeName() {
    return TEXT("");
}

FString UInventoryItem::GetAmmoString() {
    return TEXT("");
}

FPRAmmoContainerInfo UInventoryItem::GetAmmoContainerInfo() {
    return FPRAmmoContainerInfo{};
}

void UInventoryItem::GetAllAttachments(TArray<UInventoryItem*>& AttachItems) {
}

void UInventoryItem::ChangeDurabilityByRatio(const float Ratio, const EDamageCategory DamageCategory, bool& bSuccess, float& ChangedBy) {
}

void UInventoryItem::ChangeDurability(const float bY, const EDamageCategory DamageCategory, bool& bSuccess, float& ChangedBy) {
}

void UInventoryItem::ChangeAmmoContainerDurability(bool& bSuccess, float& ChangedBy) {
}

void UInventoryItem::ChamberNextAmmo() {
}

void UInventoryItem::AddAmmo(FGameplayTag AmmoTag, bool& bSuccess) {
}


