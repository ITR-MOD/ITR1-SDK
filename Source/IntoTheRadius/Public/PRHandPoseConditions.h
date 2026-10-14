#pragma once
#include "CoreMinimal.h"
#include "PRHandPoseConditions.generated.h"

USTRUCT(BlueprintType)
struct INTOTHERADIUS_API FPRHandPoseConditions {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float MaxDistanceForGrip;
    
    FPRHandPoseConditions();
};

