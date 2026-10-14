#include "PRHandPoseInfosPair.h"

FPRHandPoseInfosPair::FPRHandPoseInfosPair() {
    this->SaveType = EPRHandPoseSaveType::Tag;
    this->PoseNumber = EPRHandPoseNumber::Pose1;
    this->GeometryType = EPRHandPoseGeometryType::Point;
}

