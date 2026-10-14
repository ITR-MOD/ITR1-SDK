#include "NPCGameData.h"

FNPCGameData::FNPCGameData() {
    this->Health = 0.00f;
    this->Awareness = 0.00f;
    this->Command = 0;
    this->bHasAmmo = false;
    this->bLowAmmo = false;
    this->bIsEnemySpotted = false;
    this->bHasKnownEnemyLoc = false;
    this->bHasSquad = false;
    this->bSnipingMode = false;
    this->bIsQuest = false;
}

