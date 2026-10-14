#include "PoolActorConfig.h"

FPoolActorConfig::FPoolActorConfig() {
    this->ActorClass = NULL;
    this->InitCount = 0;
    this->MaxCount = 0;
    this->ReCreateOnDestroy = false;
}

