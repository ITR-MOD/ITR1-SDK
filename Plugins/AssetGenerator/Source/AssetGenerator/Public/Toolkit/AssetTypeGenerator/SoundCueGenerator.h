#pragma once
#include "CoreMinimal.h"
#include "UE427AssetPath.h"
#include "Toolkit/AssetTypeGenerator/SimpleAssetGenerator.h"
#include "SoundCueGenerator.generated.h"

UCLASS(MinimalAPI)
class USoundCueGenerator : public USimpleAssetGenerator {
	GENERATED_BODY()
protected:
	virtual UClass* GetAssetObjectClass() const override;
	virtual void CreateAssetPackage() override;
	virtual void OnExistingPackageLoaded() override;
	void RebuildSoundCue(class USoundCue* SoundCue);
public:
	virtual FTopLevelAssetPath GetAssetClass() override;
	virtual void PopulateStageDependencies(TArray<FPackageDependency>& OutDependencies) const override;
};