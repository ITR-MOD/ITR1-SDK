#include "FLAnimation.h"

UFLAnimation::UFLAnimation() {
}

bool UFLAnimation::GetDurationOfActiveNotifyState(const UAnimInstance* AnimInstance, const UAnimNotifyState* AnimNotifyStateInstance, float& OutDuration) {
    return false;
}

float UFLAnimation::GetAnimSequencePlayLength(const UAnimSequenceBase* Anim, const float StartTime, const bool bPlayingBackwards) {
    return 0.0f;
}


