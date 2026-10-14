#include "FLPlatform.h"

UFLPlatform::UFLPlatform() {
}

bool UFLPlatform::IsVRControllerDefault() {
    return false;
}

bool UFLPlatform::IsPlatformQuest3() {
    return false;
}

bool UFLPlatform::IsPlatformAndroid() {
    return false;
}

bool UFLPlatform::IsNativeOculus() {
    return false;
}

bool UFLPlatform::HasVRFocus() {
    return false;
}

EPRControllerType UFLPlatform::GetVRControllerType() {
    return EPRControllerType::Unknown;
}

FString UFLPlatform::GetPlatformTypeString() {
    return TEXT("");
}

EPRPlatformType UFLPlatform::GetPlatformType() {
    return EPRPlatformType::Unknown;
}

void UFLPlatform::GetPixelDensityScale(float& ScaleX, float& ScaleY) {
}

FIntPoint UFLPlatform::GetIdealRenderTargetSize() {
    return FIntPoint{};
}

EPRHmdDeviceType UFLPlatform::GetHMDDeviceType() {
    return EPRHmdDeviceType::Unknown;
}

FString UFLPlatform::GetGPUBrandName() {
    return TEXT("");
}

void UFLPlatform::GetFingerCurls(const EPRHand hand, FFingerCurls& FingerCurls) {
}

EPRControllerType UFLPlatform::GetDefaultControllerType(EPRHmdDeviceType HMDDevice) {
    return EPRControllerType::Unknown;
}

void UFLPlatform::GetCurlsAndSplaysState(bool& LeftHandState, bool& RightHandState) {
}

FString UFLPlatform::GetCPUVendorName() {
    return TEXT("");
}

FString UFLPlatform::GetCPUBrandName() {
    return TEXT("");
}


