#include "CloseCombatManagerConfig.h"

FCloseCombatManagerConfig::FCloseCombatManagerConfig() {
    this->bEnabled = false;
    this->CloseRangeRadius = 0.00f;
    this->CircleOfInterestRadius = 0.00f;
    this->MinDistanceFromPlayerToReappear = 0.00f;
    this->TeleportSearchRadius = 0.00f;
    this->Budget = 0.00f;
}

