#pragma once
#include "CoreMinimal.h"
#include "UE427AssetPath.h"
#include "Commandlets/Commandlet.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "AssetGeneratorCommandlet.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAssetGeneratorCommandlet, All, All);

UCLASS()
class ASSETGENERATOR_API UAssetGeneratorCommandlet : public UCommandlet {
	GENERATED_BODY()
public:
	UAssetGeneratorCommandlet();

	virtual int32 Main(const FString& Params) override;
private:
	void ProcessDeferredCommands();
	void ClearEmptyGamePackagesLoadedDuringDisregardGC();

	virtual IAssetRegistry& Get();
};
