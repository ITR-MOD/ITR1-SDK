#include "RadiusGameDifficulty.h"

FRadiusGameDifficulty::FRadiusGameDifficulty() {
    this->SleepRestoreHealth = false;
    this->LocationOnMap = false;
    this->ShowTips = false;
    this->Hunger = EDifficultyType::None;
    this->EnemySense = EDifficultyType::None;
    this->EnemyHealth = EDifficultyType::None;
    this->EnemyDamage = EDifficultyType::None;
    this->EnemyCount = EDifficultyType::None;
    this->ItemsDropType = EItemsDropType::Backpack;
    this->ItemSellPrice = EDifficultyType::None;
    this->WeaponShotDamage = EDifficultyType::None;
    this->AnomalyDamage = EDifficultyType::None;
    this->AnomalyAmount = EDifficultyType::None;
    this->ShowTracers = ETracersShowType::None;
    this->MissionMoneyReward = EDifficultyType::None;
    this->TideTime = EDifficultyType::None;
}

