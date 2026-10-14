#include "PRAnimPlayer.h"

UPRAnimPlayer::UPRAnimPlayer() {
    this->Pawn = NULL;
    this->bDisable = false;
    this->InputLeftPose = EHandPose::Default;
    this->InputLeftHandGeometryType = EPRHandPoseGeometryType::Point;
    this->InputLeftHandPoseNumber = EPRHandPoseNumber::Pose1;
    this->InputLeftObject = NULL;
    this->InputLeftActor = NULL;
    this->InputRightPose = EHandPose::Default;
    this->InputRightHandGeometryType = EPRHandPoseGeometryType::Point;
    this->InputRightHandPoseNumber = EPRHandPoseNumber::Pose1;
    this->InputRightObject = NULL;
    this->InputRightActor = NULL;
    this->InputDirection = 0.00f;
    this->InputSpeed = 0.00f;
    this->InputPlayerHeight = 0.00f;
    this->InputPlayerHeightScale = 0.00f;
    this->bInputRagdoll = false;
    this->PlayerMeshHeight = 183.50f;
    this->PlayerMeshEyeHeight = 171.00f;
    this->NeckOffset = 17.00f;
    this->MinPlayerHeight = 120.00f;
    this->MaxPlayerHeight = 220.00f;
}

void UPRAnimPlayer::ReceiveInputDataNative() {
}


