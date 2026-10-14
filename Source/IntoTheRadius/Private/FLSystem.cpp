#include "FLSystem.h"

UFLSystem::UFLSystem() {
}

bool UFLSystem::IsSimulate() {
    return false;
}

bool UFLSystem::IsShippingBuild() {
    return false;
}

bool UFLSystem::IsQABuild() {
    return false;
}

bool UFLSystem::IsModKitBuild() {
    return false;
}

bool UFLSystem::IsEditor() {
    return false;
}

bool UFLSystem::IsDevelopmentBuild() {
    return false;
}

bool UFLSystem::IsBuild() {
    return false;
}

FString UFLSystem::GetRelevantLanguage() {
    return TEXT("");
}

void UFLSystem::GetPuTimers(float& RawFrameTime, float& RawGameThreadTime, float& RawrenderThreadTime, float& RawGPUFrameTime) {
}

FString UFLSystem::GetProjectVersion() {
    return TEXT("");
}

FString UFLSystem::GetProjectRevision() {
    return TEXT("");
}

FString UFLSystem::GetProjectBeta() {
    return TEXT("");
}

FString UFLSystem::GetInstallID() {
    return TEXT("");
}

FString UFLSystem::GetFullVersionString() {
    return TEXT("");
}

FString UFLSystem::GetClientVersionString() {
    return TEXT("");
}


