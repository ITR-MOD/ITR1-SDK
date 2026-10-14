#include "GameConfig.h"

UGameConfig::UGameConfig() {
    this->LootSpawnDistance = 10000.00f;
    this->DisableInputInTransitionMonologue = true;
    this->SkipTransitionMonologueInProd = false;
    this->LoadingHintsChangeTime = 10.00f;
    this->LoadingHintsPSOUpdateTime = 2.00f;
    this->MissionAbortCooldownSeconds = 3600.00f;
    this->BulletsTag = TEXT("Bullet");
    this->RealMinutesInGameHour = 2.00f;
    this->TidePeriodInGameHours = 96.00f;
    this->TideSignalInGameSeconds = 600.00f;
    this->FogReEnterGameHoursSkip = 2.00f;
    this->OnlyBigClustersAreSpecial = true;
    this->AutosaveTimeRealSeconds = 60.00f;
    this->AutosaveOnIronmanDeath = true;
    this->AutosaveIconShownSeconds = 3.00f;
    this->UseSaveMessageIcon = false;
    this->MinimumPlayerLevelToSkipTutorials = 3;
    this->ActivationDistanceToPlayer = 25000.00f;
    this->CriticalDamageMultiplier = 4.00f;
    this->AnomalySpawnMultiplier = 1.00f;
    this->AnomalySpawnBlockCutoffRadiusMultiplier = 2.00f;
    this->AnomaliesSpawnDistance = 10000.00f;
    this->SellPricePercent = 10;
    this->RepairPriceToItemPrice = 100;
    this->RepairSeconds = 600.00f;
}

UGameConfig* UGameConfig::GetGameConfig() {
    return NULL;
}

void UGameConfig::ApplyStartupConfig(FStartupConfig Config) {
}


