#pragma once
#include "CoreMinimal.h"
#include "FalseTrueFloat.generated.h"

USTRUCT(Atomic, BlueprintType)
struct INTOTHERADIUS_API FFalseTrueFloat {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float FalseValue;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float TrueValue;
    
    FFalseTrueFloat();
};

