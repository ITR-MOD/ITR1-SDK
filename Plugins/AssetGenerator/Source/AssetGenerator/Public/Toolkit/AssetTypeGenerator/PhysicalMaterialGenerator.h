#pragma once
#include "CoreMinimal.h"
#include "UE427AssetPath.h"
#include "Toolkit/AssetTypeGenerator/SimpleAssetGenerator.h"
#include "PhysicalMaterialGenerator.generated.h"

UCLASS(MinimalAPI)
class UPhysicalMaterialGenerator : public USimpleAssetGenerator {
	GENERATED_BODY()
protected:
	virtual UClass* GetAssetObjectClass() const override;
public:
	virtual FTopLevelAssetPath GetAssetClass() override;
};