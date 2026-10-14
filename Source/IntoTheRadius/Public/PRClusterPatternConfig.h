#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PRClusterAnomalyConfig.h"
#include "PRClusterPatternConfig.generated.h"

USTRUCT(BlueprintType)
struct INTOTHERADIUS_API FPRClusterPatternConfig {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FPRClusterAnomalyConfig> AnomalyPoints;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FVector> ArtifactPoints;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FVector BlockExtent;
    
    FPRClusterPatternConfig();
};

