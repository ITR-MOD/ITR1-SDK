#pragma once
#include "CoreMinimal.h"
#include "EnemySenseMultiplier.generated.h"

USTRUCT(Atomic, BlueprintType)
struct INTOTHERADIUS_API FEnemySenseMultiplier {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float HearingRadiusRange;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float SightDistanceRange;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float SightAnglesHorizontal;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float SightAnglesVertical;
    
    FEnemySenseMultiplier();
};

