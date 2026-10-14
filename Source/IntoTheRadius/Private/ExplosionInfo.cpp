#include "ExplosionInfo.h"

FExplosionInfo::FExplosionInfo() {
    this->MaxDamage = 0.00f;
    this->MaxRange = 0.00f;
    this->HasImpulse = false;
    this->NoiseDistance = 0.00f;
    this->ExplosionSound = NULL;
    this->IsForceTrace = false;
}

