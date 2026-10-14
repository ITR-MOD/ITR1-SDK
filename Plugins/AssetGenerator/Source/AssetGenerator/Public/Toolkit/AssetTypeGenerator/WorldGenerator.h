#pragma once
#include "CoreMinimal.h"
#include "UE427AssetPath.h"
#include "Toolkit/AssetGeneration/AssetTypeGenerator.h"
#include "WorldGenerator.generated.h"

UCLASS()
class ASSETGENERATOR_API UWorldGenerator : public UAssetTypeGenerator {
    GENERATED_BODY()
protected:
    virtual void CreateAssetPackage() override;
    virtual void PopulateAssetWithData() override;
public:
    virtual FTopLevelAssetPath GetAssetClass() override;
    virtual void PopulateStageDependencies(TArray<FPackageDependency>& OutDependencies) const override;
};
