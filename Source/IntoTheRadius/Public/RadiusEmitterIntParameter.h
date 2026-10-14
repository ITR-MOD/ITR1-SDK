#pragma once
#include "CoreMinimal.h"
#include "RadiusEmitterIntParameter.generated.h"

USTRUCT(BlueprintType)
struct FRadiusEmitterIntParameter {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FName Name;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    int32 Value;
    
    INTOTHERADIUS_API FRadiusEmitterIntParameter();
};

