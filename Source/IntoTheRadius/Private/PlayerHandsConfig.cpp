#include "PlayerHandsConfig.h"

UPlayerHandsConfig::UPlayerHandsConfig() {
    this->PistolsStickBreakDistance = 100.00f;
    this->PrimaryHandPosesTable = NULL;
    this->PrimaryTriggeredHandPosesTable = NULL;
    this->PrimarySafetyHandPosesTable = NULL;
    this->SecondaryHandPosesTable = NULL;
    this->ThrowHandPosesTable = NULL;
    this->SlideHandPosesTable = NULL;
}

FTransform UPlayerHandsConfig::ToTransform(FPRCleanTransform cleanTransform) {
    return FTransform{};
}

void UPlayerHandsConfig::ShowDeviceModel(USceneComponent* Parent, UPrimitiveComponent* DisplayComponent, EControllerHand hand, UPrimitiveComponent*& newDisplayComponent) {
}

void UPlayerHandsConfig::SaveToAsset() {
}

void UPlayerHandsConfig::SavePoseDefault(TMap<EPRControllerType, FPRCleanTransform> leftItemShifts, TMap<EPRControllerType, FPRCleanTransform> rightItemShifts, FPRHandPoseInfosPair handsData) {
}

bool UPlayerHandsConfig::HaveHandsPoseForTagByGeometryTypeAndPoseNumber(EPRHandPoseType PoseType, FGameplayTag Tag, EPRHandPoseGeometryType GeometryType, EPRHandPoseNumber PoseNumber) const {
    return false;
}

bool UPlayerHandsConfig::HaveHandsPoseForTag(const EPRHandPoseType PoseType, const FGameplayTag Tag) const {
    return false;
}

bool UPlayerHandsConfig::HaveHandsPoseForStaticMeshByGeometryTypeAndPoseNumber(const EPRHandPoseType PoseType, UStaticMesh* StaticMesh, const EPRHandPoseGeometryType GeometryType, const EPRHandPoseNumber PoseNumber) const {
    return false;
}

bool UPlayerHandsConfig::HaveHandsPoseForStaticMesh(const EPRHandPoseType PoseType, UStaticMesh* StaticMesh) const {
    return false;
}

bool UPlayerHandsConfig::HaveHandsPoseForObjectByGeometryTypeAndPoseNumber(EPRHandPoseType PoseType, UObject* Object, EPRHandPoseGeometryType GeometryType, EPRHandPoseNumber PoseNumber) const {
    return false;
}

bool UPlayerHandsConfig::HaveHandsPoseForObject(EPRHandPoseType PoseType, UObject* Object) const {
    return false;
}

FTransform UPlayerHandsConfig::GetShiftForController(const FPRHandPoseInfo& Info, const EPRControllerType& Controller) {
    return FTransform{};
}

void UPlayerHandsConfig::GetPointShiftForObjectByGeometryTypeAndPoseNumber(EPRHandPoseType PoseType, EPRHandPoseGeometryType GeometryType, EPRHandPoseNumber PoseNumber, UObject* Object, bool Left, const FTransform& handTransform, bool& success, FTransform& Result, FTransform& WorldTransform, FPRHandPoseConditions& HandPoseConditions) const {
}

void UPlayerHandsConfig::GetPointShiftForObject(EPRHandPoseType PoseType, UObject* Object, bool Left, const FTransform& handTransform, bool& success, FTransform& Result, FTransform& WorldTransform) const {
}

UPlayerHandsConfig* UPlayerHandsConfig::GetPlayerHandsConfig() {
    return NULL;
}

void UPlayerHandsConfig::GetItemShiftForController(const TMap<EPRControllerType, FTransform>& shiftPerController, const EPRControllerType& Controller, FTransform& Shift) {
}

FPRHandPoseInfosPair UPlayerHandsConfig::GetHandsPoseForTagByGeometryTypeAndPoseNumber(EPRHandPoseType PoseType, FGameplayTag Tag, EPRHandPoseGeometryType GeometryType, EPRHandPoseNumber PoseNumber) const {
    return FPRHandPoseInfosPair{};
}

FPRHandPoseInfosPair UPlayerHandsConfig::GetHandsPoseForTag(EPRHandPoseType PoseType, FGameplayTag Tag) const {
    return FPRHandPoseInfosPair{};
}

FPRHandPoseInfosPair UPlayerHandsConfig::GetHandsPoseForStaticMeshByGeometryTypeAndPoseNumber(EPRHandPoseType PoseType, UStaticMesh* StaticMesh, EPRHandPoseGeometryType GeometryType, EPRHandPoseNumber PoseNumber) const {
    return FPRHandPoseInfosPair{};
}

FPRHandPoseInfosPair UPlayerHandsConfig::GetHandsPoseForStaticMesh(EPRHandPoseType PoseType, UStaticMesh* StaticMesh) const {
    return FPRHandPoseInfosPair{};
}

FPRHandPoseInfosPair UPlayerHandsConfig::GetHandsPoseForObjectByGeometryTypeAndPoseNumber(EPRHandPoseType PoseType, UObject* Object, EPRHandPoseGeometryType GeometryType, EPRHandPoseNumber PoseNumber) const {
    return FPRHandPoseInfosPair{};
}

FPRHandPoseInfosPair UPlayerHandsConfig::GetHandsPoseForObject(EPRHandPoseType PoseType, UObject* Object) const {
    return FPRHandPoseInfosPair{};
}

void UPlayerHandsConfig::GetHandShiftForObjectByGeometryTypeAndPoseNumber(EPRHandPoseType PoseType, EPRHandPoseGeometryType GeometryType, EPRHandPoseNumber PoseNumber, UObject* Object, bool Left, const FTransform& handTransform, bool& success, FTransform& Result, FTransform& WorldTransform, FPRHandPoseConditions& HandPoseConditions) const {
}

void UPlayerHandsConfig::GetHandShiftForObject(EPRHandPoseType PoseType, UObject* Object, bool Left, const FTransform& handTransform, bool& success, FTransform& Result, FTransform& WorldTransform) const {
}

void UPlayerHandsConfig::GetControllerShifts(const EPRControllerType Controller, FTransform& LeftShift, FTransform& RightShift) {
}

void UPlayerHandsConfig::GetBestPointShiftForObjectByConditions(EPRHandPoseType PoseType, UObject* Object, bool Left, const FTransform& handTransform, bool useGripLimit, const FVector& handRootLocation, const UGripMotionControllerComponent* MotionController, bool& success, FTransform& Result, FTransform& WorldTransform, EPRHandPoseGeometryType& bestGeometryType, EPRHandPoseNumber& bestPoseNumber) const {
}

void UPlayerHandsConfig::GetBestPointShiftForObject(EPRHandPoseType PoseType, UObject* Object, bool Left, const FTransform& handTransform, bool& success, FTransform& Result, FTransform& WorldTransform, EPRHandPoseGeometryType& bestGeometryType, EPRHandPoseNumber& bestPoseNumber) const {
}

void UPlayerHandsConfig::GetBestHandShiftForObjectByConditions(EPRHandPoseType PoseType, UObject* Object, bool bLeft, const FTransform& CurrentHandTransform, bool bUseArmLengthLimit, const FVector& HandShoulderLocation, const UGripMotionControllerComponent* MotionController, bool& bSuccess, FTransform& PoseRelativeTransform, FTransform& PoseWorldTransform, EPRHandPoseGeometryType& PoseGeometryType, EPRHandPoseNumber& PoseNumber) const {
}

void UPlayerHandsConfig::GetBestHandShiftForObject(EPRHandPoseType PoseType, UObject* Object, bool Left, const FTransform& handTransform, bool& success, FTransform& Result, FTransform& WorldTransform, EPRHandPoseGeometryType& bestGeometryType, EPRHandPoseNumber& bestPoseNumber) const {
}

FPRCleanTransform UPlayerHandsConfig::FromTransform(FTransform Transform) {
    return FPRCleanTransform{};
}


