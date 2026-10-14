#include "CoverRuntimeDataHolder.h"

UCoverRuntimeDataHolder::UCoverRuntimeDataHolder() {
    this->ShippingDebugActor = NULL;
}

void UCoverRuntimeDataHolder::ToggleDebug() {
}

void UCoverRuntimeDataHolder::OnLevelUnloaded() {
}

bool UCoverRuntimeDataHolder::GetDebugEnabled() const {
    return false;
}

UCoverRuntimeDataHolder* UCoverRuntimeDataHolder::GetCoverRuntimeDataHolder() {
    return NULL;
}

void UCoverRuntimeDataHolder::DrawShippingDebug(const UObject* WorldContextObject, const FVector CurrentLocation, float Radius) {
}

void UCoverRuntimeDataHolder::DebugDraw(const UObject* WorldContextObject) {
}


