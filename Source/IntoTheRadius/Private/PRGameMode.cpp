#include "PRGameMode.h"

APRGameMode::APRGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->State = EPRGameModeState::Init;
    this->bRespawnInventory = true;
    this->RadiusPlayerPawn = NULL;
    this->bSleepAllowed = true;
    this->bForcePause = false;
    this->Autosave = false;
}

void APRGameMode::TogglePause() {
}


void APRGameMode::SetGameplayTimeDilation(const float Dilation) {
}


void APRGameMode::ResetAutosaveTimer_Native() {
}

void APRGameMode::RecallLevelLoaded() {
}

void APRGameMode::PreparePlayerForFinalChoice() {
}









void APRGameMode::OnLevelUnloaded() {
}



void APRGameMode::OnLevelLoaded() {
}



void APRGameMode::OnGameStarted() {
}



void APRGameMode::OnEditorLevelLoaded() {
}

FString APRGameMode::MakeActorUID(AActor* Actor) {
    return TEXT("");
}


void APRGameMode::GetPRGameMode(APRGameMode*& GameMode) {
}

void APRGameMode::GetGameplayTimeDilation(float& Dilation) const {
}

APREnvironment* APRGameMode::GetCurrentEnvironment() {
    return NULL;
}

FString APRGameMode::GetActorUID(AActor* Actor) {
    return TEXT("");
}

void APRGameMode::FinishTransfer() {
}


void APRGameMode::DoTransferToLevel() {
}

void APRGameMode::DoStart() {
}

void APRGameMode::DoPrepare() {
}

void APRGameMode::DoEndTransition() {
}

void APRGameMode::ContinueTransitionLoad() {
}

void APRGameMode::BeginWaitingForPSO() {
}


