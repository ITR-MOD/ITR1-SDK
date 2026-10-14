#include "PRMapMarker.h"

APRMapMarker::APRMapMarker(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->MarkerType = EMapMarker::None;
}

bool APRMapMarker::IsConditionsPassed() {
    return false;
}


