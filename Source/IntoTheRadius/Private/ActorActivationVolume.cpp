#include "ActorActivationVolume.h"

AActorActivationVolume::AActorActivationVolume(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->bGenerateOverlapEventsDuringLevelStreaming = true;
    this->bColored = true;
}


