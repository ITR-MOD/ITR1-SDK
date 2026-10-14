#pragma once
#include "CoreMinimal.h"
#include "UE427AssetPath.h"
#include "SimpleAssetGenerator.h"
#include "PhysicsAssetGenerator.generated.h"

UCLASS(MinimalAPI)
class UPhysicsAssetGenerator : public USimpleAssetGenerator {
	GENERATED_BODY()
protected:
	virtual UClass* GetAssetObjectClass() const override;

	/*virtual void PopulateSimpleAssetWithData(UObject* Asset) override;
	virtual bool IsSimpleAssetUpToDate(UObject* Asset) const override;

	static bool CheckMatchesName(FString& AssetName);*/
public:
	virtual FTopLevelAssetPath GetAssetClass() override;
};
