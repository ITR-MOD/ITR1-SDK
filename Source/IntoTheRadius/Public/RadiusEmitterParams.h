#pragma once
#include "CoreMinimal.h"
#include "RadiusEmitterIntParameter.h"
#include "RadiusEmitterVectorParameter.h"
#include "RadiusEmitterParams.generated.h"

USTRUCT(BlueprintType)
struct FRadiusEmitterParams {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FRadiusEmitterVectorParameter> VectorParameters;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FRadiusEmitterIntParameter> FloatParameters;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FRadiusEmitterIntParameter> IntParameters;
    
    INTOTHERADIUS_API FRadiusEmitterParams();
};

