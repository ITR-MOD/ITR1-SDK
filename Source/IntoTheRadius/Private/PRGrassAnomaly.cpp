#include "PRGrassAnomaly.h"

APRGrassAnomaly::APRGrassAnomaly(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->MinScaleValue = -20.00f;
    this->MaxScaleValue = 1.00f;
    this->ShrinkRatio = 0.03f;
    this->GrowthRatio = 0.05f;
    this->DamageHeight = 0.20f;
}

void APRGrassAnomaly::ReduceComponent(const int32 Index, const float Value) {
}

void APRGrassAnomaly::Initialize() {
}


