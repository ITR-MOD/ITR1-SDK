#pragma once
#include "CoreMinimal.h"
#include "UE427AssetPath.h"
#include "Toolkit/AssetGeneration/AssetTypeGenerator.h"
#include "Toolkit/AssetTypeGenerator/SimpleAssetGenerator.h"
#include "RecoveredEffectGenerators.generated.h"

UCLASS()
class URecoveredControlRigGenerator : public UAssetTypeGenerator {
    GENERATED_BODY()
protected:
    virtual void CreateAssetPackage() override;
    virtual void OnExistingPackageLoaded() override;
public:
    virtual FTopLevelAssetPath GetAssetClass() override;
};

UCLASS()
class URecoveredNiagaraGenerator : public UAssetTypeGenerator {
    GENERATED_BODY()
protected:
    virtual void CreateAssetPackage() override;
    virtual void OnExistingPackageLoaded() override;
public:
    virtual FTopLevelAssetPath GetAssetClass() override;
};

UCLASS()
class URecoveredPoseAssetGenerator : public USimpleAssetGenerator {
    GENERATED_BODY()
protected:
    virtual UClass* GetAssetObjectClass() const override;
    virtual void PopulateSimpleAssetWithData(UObject* Asset) override;
public:
    virtual FTopLevelAssetPath GetAssetClass() override;
};
