#pragma once
#include "CoreMinimal.h"
#include "EDamageCategory.h"
#include "ArmorParameters.generated.h"

USTRUCT(BlueprintType)
struct INTOTHERADIUS_API FArmorParameters {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<EDamageCategory, float> ArmorCoefficients;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float AdditionalNoiseCoefficient;
    
    FArmorParameters();
};

