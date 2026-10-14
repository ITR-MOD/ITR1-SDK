#include "PRHolderMark.h"

APRHolderMark::APRHolderMark(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->ActorInLeftHand = NULL;
    this->ActorInRightHand = NULL;
    this->holsterRef = NULL;
}






AActor* APRHolderMark::GetActorInRightHand() {
    return NULL;
}

AActor* APRHolderMark::GetActorInLeftHand() {
    return NULL;
}


