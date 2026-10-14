#include "AIEdPerceptionComp.h"

UAIEdPerceptionComp::UAIEdPerceptionComp(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
}

bool UAIEdPerceptionComp::SetSightHalfAngle(float HalfAngle) {
    return false;
}

bool UAIEdPerceptionComp::SetSightDistance(float Distance) {
    return false;
}

bool UAIEdPerceptionComp::SetHearingDistance(float Distance) {
    return false;
}

float UAIEdPerceptionComp::GetSightHalfAngle() {
    return 0.0f;
}

float UAIEdPerceptionComp::GetSightDistance() {
    return 0.0f;
}

float UAIEdPerceptionComp::GetHearingDistance() {
    return 0.0f;
}


