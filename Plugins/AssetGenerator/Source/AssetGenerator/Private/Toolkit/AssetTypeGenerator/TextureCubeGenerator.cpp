#include "Toolkit/AssetTypeGenerator/TextureCubeGenerator.h"
#include "UE427AssetPath.h"
#include "Engine/TextureCube.h"

TSubclassOf<UTexture> UTextureCubeGenerator::GetTextureClass() {
	return UTextureCube::StaticClass();
}

FTopLevelAssetPath UTextureCubeGenerator::GetAssetClass() {
	return FTopLevelAssetPath(UTextureCube::StaticClass());
}
