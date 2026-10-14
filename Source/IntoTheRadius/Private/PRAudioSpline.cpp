#include "PRAudioSpline.h"
#include "Components/AudioComponent.h"
#include "Components/SplineComponent.h"

APRAudioSpline::APRAudioSpline(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
    this->Audio = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio"));
    this->LoopBeginMin = 0.00f;
    this->LoopBeginMax = 0.00f;
}


