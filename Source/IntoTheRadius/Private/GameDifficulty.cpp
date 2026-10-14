#include "GameDifficulty.h"

FGameDifficulty::FGameDifficulty() {
    this->SleepRestoreHealth = false;
    this->LocationOnMap = false;
    this->EnemySense = EEnemySenseType::Reduced;
    this->ShowTracers = false;
    this->ShowTips = false;
    this->ItemsDropType = EItemsDropType::Backpack;
}

