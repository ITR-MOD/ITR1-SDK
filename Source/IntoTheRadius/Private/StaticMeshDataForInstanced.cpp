#include "StaticMeshDataForInstanced.h"

FStaticMeshDataForInstanced::FStaticMeshDataForInstanced() {
    this->CountOfStaticMeshInstances = 0;
    this->MaxCullDistance = 0.00f;
    this->EnableCollisionOverlapEvents = false;
    this->ComponentMobility = EComponentMobility::Static;
}

