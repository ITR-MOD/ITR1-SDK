#pragma once
#include "CoreMinimal.h"
#include "UE427AssetPath.h"
#include "Toolkit/AssetTypeGenerator/SimpleAssetGenerator.h"
#include "DataAssetGenerator.generated.h"

UCLASS(MinimalAPI)
class UDataAssetGenerator : public USimpleAssetGenerator {
	GENERATED_BODY()
protected:
	virtual UClass* GetAssetObjectClass() const override;
	virtual void CreateAssetPackage() override;
	virtual void OnExistingPackageLoaded() override;
	virtual void PopulateAssetWithData() override;
	bool DefersAudioProperties() const;
public:
	virtual void PopulateStageDependencies(TArray<FPackageDependency>& OutDependencies) const override;
	virtual FTopLevelAssetPath GetAssetClass() override;
	virtual void GetAdditionallyHandledAssetClasses(TArray<FTopLevelAssetPath>& OutExtraAssetClasses) override;
};
