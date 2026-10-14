#include "AmmoInfo.h"

FAmmoInfo::FAmmoInfo() {
    this->ShellMesh = NULL;
    this->WholeMesh = NULL;
    this->ShortInfo = false;
    this->VelocityMultiplier = 0.00f;
    this->DamageType = NULL;
    this->Penetration = 0.00f;
    this->BounceMax = 0;
    this->ArmorPiercing = 0.00f;
    this->DamageModifier = 0.00f;
    this->SpeedDropCurve = NULL;
}

