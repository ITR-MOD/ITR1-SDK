#pragma once
#include "CoreMinimal.h"
#include "MinimapCliffMappingConfig.generated.h"

USTRUCT(Atomic, BlueprintType)
struct INTOTHERADIUS_API FMinimapCliffMappingConfig {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float scaleThreshold;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float scaleRatio;
    
    FMinimapCliffMappingConfig();
};

