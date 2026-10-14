#include "PRPlayerController.h"
#include "PRCheatManager.h"

APRPlayerController::APRPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->CheatClass = UPRCheatManager::StaticClass();
    this->ClickEventKeys.AddDefaulted(1);
}

void APRPlayerController::ReportReady() {
}

bool APRPlayerController::IsPossessingPlayer() const {
    return false;
}


