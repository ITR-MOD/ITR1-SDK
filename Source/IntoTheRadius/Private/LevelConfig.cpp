#include "LevelConfig.h"

FLevelConfig::FLevelConfig() {
    this->bTutorialLevel = false;
    this->StopGameTimeTick = false;
    this->ReloadOnTideHappening = false;
    this->InventoryAllowed = false;
    this->SleepAllowed = false;
    this->SaveOnExit = false;
    this->Autosave = false;
    this->SaveOnLoad = false;
    this->MapMaterial = NULL;
    this->MapLegendMaterial = NULL;
    this->MapScale = 0.00f;
    this->MapRotation = 0.00f;
    this->MaxLevelRadius = 0.00f;
    this->FullPlayerStats = false;
}

