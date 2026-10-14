#include "MovementSettings.h"

FMovementSettings::FMovementSettings() {
    this->WalkSpeed = 0.00f;
    this->RunSpeed = 0.00f;
    this->SprintSpeed = 0.00f;
    this->MovementCurve = NULL;
    this->RotationRateCurve = NULL;
}

