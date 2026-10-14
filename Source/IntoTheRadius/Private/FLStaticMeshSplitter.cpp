#include "FLStaticMeshSplitter.h"

UFLStaticMeshSplitter::UFLStaticMeshSplitter() {
}

TArray<AActor*> UFLStaticMeshSplitter::SplitStaticMeshByMaterial(AActor* inputActor) {
    return TArray<AActor*>();
}

void UFLStaticMeshSplitter::SetLodScreenSize(UStaticMeshComponent* StaticMeshComponent, int32 LODIndex, float ScreenSize) {
}

void UFLStaticMeshSplitter::SetLightMapResolution(UStaticMeshComponent* StaticMeshComponent, int32 Resolution) {
}

void UFLStaticMeshSplitter::SetLightmapCoordinateIndex(UStaticMesh* StaticMesh, int32 coordinateIndex) {
}

bool UFLStaticMeshSplitter::SetCustomDataInTexcoord(UStaticMeshComponent* StaticMeshComponent, int32 textureCoordIndex, FVector2D customData) {
    return false;
}

void UFLStaticMeshSplitter::RemoveCollisionFromLodSections(UStaticMeshComponent* StaticMeshComponent) {
}

FVector2D UFLStaticMeshSplitter::PackTintAndTextureIndex(FLinearColor Tint, int32 TextureIndex) {
    return FVector2D{};
}

void UFLStaticMeshSplitter::CreateEmptyLod(UStaticMeshComponent* StaticMeshComponent) {
}

void UFLStaticMeshSplitter::CreateCopyOfExistingLod(UStaticMeshComponent* StaticMeshComponent, int32 sourceLodIndex) {
}

void UFLStaticMeshSplitter::BuildStaticMesh(UStaticMesh* StaticMesh) {
}


