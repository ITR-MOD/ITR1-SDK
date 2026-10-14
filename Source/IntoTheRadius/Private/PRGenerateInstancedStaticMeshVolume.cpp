#include "PRGenerateInstancedStaticMeshVolume.h"

APRGenerateInstancedStaticMeshVolume::APRGenerateInstancedStaticMeshVolume(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->bIsEditorOnlyActor = true;
    this->bClearVarsAfterEvent = true;
    this->ExcludedStaticMeshList.AddDefaulted(2);
    this->bUseMeshDefaultMaterial = false;
    this->bHideDuplicated = true;
    this->bCreateHism = true;
    this->MinSmCountToCreateInstanced = 10;
}

void APRGenerateInstancedStaticMeshVolume::ShowHiddenStaticMeshComponents() {
}

void APRGenerateInstancedStaticMeshVolume::ShowAllGeneratedInstanced() {
}

void APRGenerateInstancedStaticMeshVolume::RestoreHidden() {
}

void APRGenerateInstancedStaticMeshVolume::RehideHiddenStaticMeshComponents() {
}

void APRGenerateInstancedStaticMeshVolume::HideAllGeneratedInstanced() {
}

void APRGenerateInstancedStaticMeshVolume::GatherMulitipleSmActors() {
}

void APRGenerateInstancedStaticMeshVolume::DestroyAllGeneratedInstanced() {
}

void APRGenerateInstancedStaticMeshVolume::CreateMultipleInstanced() {
}

void APRGenerateInstancedStaticMeshVolume::CreateInstanced() {
}


