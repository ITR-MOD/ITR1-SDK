#include "SystemConfig.h"

USystemConfig::USystemConfig() {
    this->EmptyMapMaterial = NULL;
    this->CommonLevelName = TEXT("L_Common");
    this->TransitionSublevelName = TEXT("L_Transition");
    this->FogFadeTime = 2.00f;
    this->FogFadeDensityMultiplier = 10.00f;
    this->TransitionTimeSkipHoursPerSecond = 1.00f;
    this->DisableFirstLaunchInEditor = true;
    this->DetectorWorkDistance = 500.00f;
    this->AmmoContainersLockDelay = 0.60f;
    this->AmmoContainersLoadingDelay = 0.30f;
    this->LoadShortNPCGameData = false;
    this->IngameMenuVerticalShift = 5000.00f;
    this->FallThroughFloorZ = -22222.00f;
    this->DoNotDropItemsFromBackpack = true;
    this->BulletsFlyBySoundDistance = 450.00f;
    this->NPCDiscoverDistance = 450.00f;
    this->AnomalyDiscoverDistance = 450.00f;
    this->LowLevelGPUs.AddDefaulted(5);
    this->HighLevelGPUs.AddDefaulted(3);
    this->PlayerPositionAdjustmentInMenu = EPlayerTeleportAdjustType::LocationAndRotation;
    this->PlayerPositionAdjustmentInTransition = EPlayerTeleportAdjustType::LocationAndRotation;
    this->MainMenuFollowsPlayer = false;
    this->bCountEditorAsEarlyAccess = true;
    this->AwaitForDetectionOnStartup = false;
    this->ExitIfUnsupportedControllers = true;
    this->SteamBetaBranchName = TEXT("active-beta");
    this->SteamHideRevisionBranches.AddDefaulted(2);
    this->ClustersSpawnDistance = 100000.00f;
    this->ClearProfileSavesOnFinalSuicide = true;
    this->LevelTransferStepsDelay = 0.20f;
    this->NPCSpawnMinDistanceFromPlayer = 1000.00f;
    this->NPCSpawnZShift = 90.00f;
    this->NPCMaxDistanceFromSpawner = 10000.00f;
    this->NPCSpawnerRestoreRadius = 1000.00f;
}

bool USystemConfig::IsTutorialLevel(const FGameplayTag& LevelTag) const {
    return false;
}

bool USystemConfig::IsLevelTagValid(FGameplayTag LevelTag) const {
    return false;
}

USystemConfig* USystemConfig::GetSystemConfig() {
    return NULL;
}

FText USystemConfig::GetLevelName(const FGameplayTag& LevelTag, const FText& Placeholder) const {
    return FText::GetEmpty();
}

void USystemConfig::GetLevelConfig(FGameplayTag LevelTag, FLevelConfig& Config) const {
}

void USystemConfig::GetCurrentLevelConfig(FLevelConfig& Config) const {
}


