#pragma once
#include "CoreMinimal.h"
#include "UE427AssetPath.h"
#include "Toolkit/AssetGeneration/AssetTypeGenerator.h"
#include "SimpleAssetGenerator.generated.h"

UCLASS(Abstract)
class ASSETGENERATOR_API USimpleAssetGenerator : public UAssetTypeGenerator {
	GENERATED_BODY()
protected:
	FString GetParticlePlaceholderReason() const;
	FString ParticlePlaceholderReason;
	virtual UClass* GetAssetObjectClass() const PURE_VIRTUAL(GetAssetObjectClass, return NULL;);
	virtual void CreateAssetPackage() override;
	virtual void OnExistingPackageLoaded() override;
	
	virtual void PopulateSimpleAssetWithData(UObject* Asset);
	virtual bool IsSimpleAssetUpToDate(UObject* Asset) const;
public:
	virtual void PopulateStageDependencies(TArray<FPackageDependency>& OutDependencies) const override;
};
