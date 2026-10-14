#include "GripOverlap.h"

FGripOverlap::FGripOverlap() {
    this->Object = NULL;
    this->ImplementsInterface = false;
    this->CanBeClimbed = false;
    this->OverlapedPrimitive = NULL;
    this->IsDistanceGrip = false;
    this->IsSlotGrip = false;
    this->TriggerPrimitive = NULL;
    this->GeometryType = EPRHandPoseGeometryType::Point;
    this->PoseNumer = EPRHandPoseNumber::Pose1;
}

