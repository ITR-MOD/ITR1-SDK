#include "PlayerGameData.h"

UPlayerGameData::UPlayerGameData() {
    this->Health = 100.00f;
    this->MaxHealth = 100.00f;
    this->Stamina = 100.00f;
    this->MaxStamina = 100.00f;
    this->Rest = 100.00f;
    this->MaxRest = 100.00f;
    this->Hunger = 0.00f;
    this->MaxHunger = 100.00f;
    this->Nutrition = 100.00f;
    this->MaxNutrition = 100.00f;
    this->Money = 0;
    this->PlayerLevel = 0;
}

FString UPlayerGameData::ToString() const {
    return TEXT("");
}

void UPlayerGameData::SavePosition(const FGameplayTag InLevel, const FTransform Position, const FVector Velocity) {
}

void UPlayerGameData::SaveDroppedItems(const FGameplayTag InLevel, const FTransform Location, const TArray<FString> ItemIds) {
}

void UPlayerGameData::ResetStats(const bool bAlive) {
}

void UPlayerGameData::ResetPosition() {
}

void UPlayerGameData::RemoveDroppedItem(const FGameplayTag InLevel, const FString& ItemId, bool& bEmpty) {
}

void UPlayerGameData::GetPosition(const FGameplayTag InLevel, FTransform& Position, FVector& Velocity, bool& hasSaved) const {
}

TArray<FItemDropInfo> UPlayerGameData::GetDroppedItems(const FGameplayTag InLevel) {
    return TArray<FItemDropInfo>();
}


