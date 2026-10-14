#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RadiusEmitterVectorParameter.generated.h"

USTRUCT(BlueprintType)
struct FRadiusEmitterVectorParameter {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FName Name;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FVector Value;
    
    INTOTHERADIUS_API FRadiusEmitterVectorParameter();
};

