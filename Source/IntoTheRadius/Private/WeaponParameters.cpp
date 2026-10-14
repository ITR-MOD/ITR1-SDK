#include "WeaponParameters.h"

FWeaponParameters::FWeaponParameters() {
    this->FireRate = 0.00f;
    this->ShotLoudness = 0.00f;
    this->MuzzleVelocity = 0.00f;
    this->AmmoDamage = 0.00f;
    this->DurabilityDamagePerShot = 0.00f;
    this->DurabilityDamageAutoFireCoefficient = 0.00f;
    this->Reliability = 0.00f;
    this->JamCurve = NULL;
    this->bDisableSmoothing = false;
    this->TwoHandSmoothingAlpha = 0.00f;
    this->TwoHandSmoothingDistance = 0.00f;
    this->BurstModeCount = 0;
}

