#include "PRSpiderNpcAnimInstance.h"

UPRSpiderNpcAnimInstance::UPRSpiderNpcAnimInstance() {
    this->RootSpinInDegPerSec = 360.00f;
    this->IdleLegBendInDeg = 70.00f;
    this->IdleBendPercentAtLegRoot = 0.10f;
    this->MovingLegBendInDeg = 170.00f;
    this->MovingBendPercentAtLegRoot = 0.70f;
    this->BendDeltaInDegPerSec = 1440.00f;
    this->bJitterEnabled = true;
    this->IdleJitterLerpCycleInSec = 0.15f;
    this->IdleJitterRadius = 5.00f;
    this->MovingJitterLerpCycleInSec = 0.05f;
    this->MovingJitterRadius = 10.00f;
    this->bVelocityDragEnabled = true;
    this->VelocityDragFactor = 10.00f;
    this->HighVelocityDragFactor = 20.00f;
    this->HighVelocityThreshold = 700.00f;
    this->DragDeltaPerSec = 80.00f;
}


