#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RadiusEmitterParams.h"
#include "RadiusEmitterInfo.generated.h"

class UParticleSystem;

USTRUCT(BlueprintType)
struct FRadiusEmitterInfo {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    UParticleSystem* Template;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FVector ReflectionVector;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FRadiusEmitterParams Parameters;
    
    INTOTHERADIUS_API FRadiusEmitterInfo();
};

