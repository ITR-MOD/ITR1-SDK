#include "EnvConfigurator.h"

AEnvConfigurator::AEnvConfigurator(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->bIsEditorOnlyActor = true;
    this->Sun = NULL;
    this->Sky = NULL;
    this->SkyLight = NULL;
    this->Fog = NULL;
    this->RainParticles = NULL;
    this->ShadowLight = NULL;
    this->ColorSun = NULL;
    this->ColorFog = NULL;
    this->ColorSky = NULL;
}


