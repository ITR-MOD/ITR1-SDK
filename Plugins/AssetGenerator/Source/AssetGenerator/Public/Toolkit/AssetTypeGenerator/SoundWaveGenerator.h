#pragma once
#include "Toolkit/AssetTypeGenerator/SimpleAssetGenerator.h"
#include "SoundWaveGenerator.generated.h"

UCLASS()
class ASSETGENERATOR_API USoundWaveGenerator : public USimpleAssetGenerator {
	GENERATED_BODY()
protected:
	virtual UClass* GetAssetObjectClass() const override;
	virtual void CreateAssetPackage() override;
	virtual void OnExistingPackageLoaded() override;
	void ImportWave();
public:
	virtual FTopLevelAssetPath GetAssetClass() override;
};
