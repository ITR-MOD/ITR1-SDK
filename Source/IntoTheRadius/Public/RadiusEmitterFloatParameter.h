#pragma once
#include "CoreMinimal.h"
#include "RadiusEmitterFloatParameter.generated.h"

USTRUCT(BlueprintType)
struct FRadiusEmitterFloatParameter {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FName Name;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float Value;
    
    INTOTHERADIUS_API FRadiusEmitterFloatParameter();
};

