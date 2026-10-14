#pragma once
#include "CoreMinimal.h"
#include "RadiusEmitterParams.h"
#include "RadiusNiagaraEmitterInfo.generated.h"

class UNiagaraSystem;

USTRUCT(BlueprintType)
struct FRadiusNiagaraEmitterInfo {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    UNiagaraSystem* Template;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FRadiusEmitterParams Parameters;
    
    INTOTHERADIUS_API FRadiusNiagaraEmitterInfo();
};

