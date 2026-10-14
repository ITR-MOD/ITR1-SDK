#include "PRVRGripSMComponent.h"

UPRVRGripSMComponent::UPRVRGripSMComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->Stiffness = 1500.00f;
    this->Damping = 200.00f;
    this->BreakDistance = 100.00f;
    this->GripPriority = 1;
    this->bDenyGripping = false;
    this->_isHeld = false;
    this->HoldingController = NULL;
}


