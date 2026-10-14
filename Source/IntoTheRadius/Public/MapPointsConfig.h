#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "GateInfo.h"
#include "IAsset.h"
#include "TagToTransformsList.h"
#include "MapPointsConfig.generated.h"

class UMapPointsConfig;

UCLASS(Blueprintable)
class INTOTHERADIUS_API UMapPointsConfig : public UDataAsset, public IIAsset {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<FGameplayTag, FTransform> levelCenters;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<FGameplayTag, FTagToTransformsList> LevelPoints;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FGateInfo> LevelGates;
    
    UMapPointsConfig();

    UFUNCTION(BlueprintCallable, BlueprintPure)
    FTransform WorldToLevelTransform(FGameplayTag LevelTag, FTransform Transform);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FVector WorldToLevelLocation(FGameplayTag LevelTag, FVector Location);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void ResolvePoint(FGameplayTag pointTag, TArray<FGameplayTag> ExcludedPoints, FGameplayTag& ResultPoint, FGameplayTag& ResultLevel);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FTransform LevelToWorldTransform(FGameplayTag LevelTag, FTransform Transform);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FVector LevelToWorldLocation(FGameplayTag LevelTag, FVector Location);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsPointInLevel(FGameplayTag pointTag, FGameplayTag LevelTag);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsPointACluster(FGameplayTag pointTag);
    
    UFUNCTION(BlueprintCallable)
    void GetPointTransforms(FGameplayTag pointTag, bool& bSuccess, FGameplayTag& LevelTag, FTransform& RelativeTransform, FTransform& absoluteTransform);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FGameplayTag GetPointLevel(FGameplayTag pointTag) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static UMapPointsConfig* GetMapPointsConfig();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    TArray<FGateInfo> GetLevelGateInfos(const FGameplayTag LevelTag);
    

    // Fix for true pure virtual functions not being implemented
};

