#include "PRSplashDesc.h"

FPRSplashDesc::FPRSplashDesc() {
    this->bIgnoreAlpha = false;
    this->bIsDynamic = false;
    this->bIsExternal = false;
    this->bLerpToHeadsetLocal = false;
    this->LerpToHeadsetLocalSpeed = 0.00f;
    this->Texture = NULL;
    this->LeftTexture = NULL;
}

