#pragma once
#include "CoreMinimal.h"
#include "MinimapFoliageMappingConfig.generated.h"

class UFoliageType;

USTRUCT(Atomic, BlueprintType)
struct INTOTHERADIUS_API FMinimapFoliageMappingConfig {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<UFoliageType*> appliedTo;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float Scale;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool rotateToMesh;
    
    FMinimapFoliageMappingConfig();
};

