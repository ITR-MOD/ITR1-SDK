#include "Toolkit/AssetTypeGenerator/SimpleAssetGen.h"
#include "UE427AssetPath.h"
#include "Dom/JsonObject.h"

UClass* USimpleAssetGen::GetAssetObjectClass() const {
	const FString AssetClassPath = GetAssetData()->GetStringField(TEXT("AssetClass"));
	return FindObjectChecked<UClass>(NULL, *AssetClassPath);
}

FTopLevelAssetPath USimpleAssetGen::GetAssetClass() {
	return TEXT("SimpleAsset"); //UPhysicalMaterial::StaticClass()->GetFName();
}

void USimpleAssetGen::PopulateStageDependencies(TArray<FPackageDependency>& OutDependencies) const {
	if (!GetAssetData()->HasField(TEXT("SkipDependecies")) || !GetAssetData()->GetBoolField(TEXT("SkipDependecies"))) {
		if (GetCurrentStage() == EAssetGenerationStage::CONSTRUCTION) {
			PopulateReferencedObjectsDependencies(OutDependencies);
		}
	}
}