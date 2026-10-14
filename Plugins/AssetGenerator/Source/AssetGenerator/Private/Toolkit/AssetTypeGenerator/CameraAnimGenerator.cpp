#include "Toolkit/AssetTypeGenerator/CameraAnimGenerator.h"
#include "UE427AssetPath.h"
#include "Camera/CameraAnim.h"

UClass* UCameraAnimGenerator::GetAssetObjectClass() const {
	return UCameraAnim::StaticClass();
}

FTopLevelAssetPath UCameraAnimGenerator::GetAssetClass() {
	return FTopLevelAssetPath(UCameraAnim::StaticClass());
}
