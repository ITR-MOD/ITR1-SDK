#include "GameDifficultyConfig.h"

UGameDifficultyConfig::UGameDifficultyConfig() {
}

bool UGameDifficultyConfig::IsSleepRestoreHealth() const {
    return false;
}

bool UGameDifficultyConfig::IsShowTips() const {
    return false;
}

bool UGameDifficultyConfig::IsShowLocationOnMap() const {
    return false;
}

bool UGameDifficultyConfig::IsNoEnemies() const {
    return false;
}

bool UGameDifficultyConfig::IsNoAnomalies() const {
    return false;
}

float UGameDifficultyConfig::GetWeaponShotDamageMultiplier() {
    return 0.0f;
}

ETracersShowType UGameDifficultyConfig::GetTracersShowType() const {
    return ETracersShowType::None;
}

float UGameDifficultyConfig::GetTideTimeMultiplier() {
    return 0.0f;
}

float UGameDifficultyConfig::GetMissionMoneyRewardMultiplier() {
    return 0.0f;
}

float UGameDifficultyConfig::GetItemSellPriceMultiplier() {
    return 0.0f;
}

EItemsDropType UGameDifficultyConfig::GetItemsDropType() {
    return EItemsDropType::Backpack;
}

float UGameDifficultyConfig::GetHungerMultiplier() {
    return 0.0f;
}

UGameDifficultyConfig* UGameDifficultyConfig::GetGameDifficultyConfig() {
    return NULL;
}

FEnemySenseMultiplier UGameDifficultyConfig::GetEnemySense() {
    return FEnemySenseMultiplier{};
}

float UGameDifficultyConfig::GetEnemyHealthMultiplier() {
    return 0.0f;
}

float UGameDifficultyConfig::GetEnemyDamageMultiplier() {
    return 0.0f;
}

int32 UGameDifficultyConfig::GetEnemyCountModifier() {
    return 0;
}

float UGameDifficultyConfig::GetAnomalyDamageMultiplier() {
    return 0.0f;
}

int32 UGameDifficultyConfig::GetAnomalyAmountModifier() {
    return 0;
}


