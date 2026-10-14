#include "VfxConfig.h"

UVfxConfig::UVfxConfig() {
    this->ImpactClass = NULL;
    this->EmitterClass = NULL;
    this->NiagaraEmitterClass = NULL;
    this->ImpactDisableFlag = TEXT("itr.performanceDebugging.enableRemoveProjectileEffectsOptim");
    this->ImpactsTables = NULL;
    this->ImpactDefaultRow = TEXT("SurfaceType_Default");
    this->ImpactDefaultVolumnMulti = 3.00f;
    this->ImpactDefaultSound = NULL;
    this->ImpactDefaultAttenuation = NULL;
    this->ImpactSpeedScale = 500.00f;
    this->BulletTracersStartDelay = 0.00f;
}

UVfxConfig* UVfxConfig::GetVfxConfig() {
    return NULL;
}


