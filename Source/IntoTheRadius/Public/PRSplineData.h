#pragma once
#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "PRCleanTransform.h"
#include "PRSplineData.generated.h"

USTRUCT(BlueprintType)
struct INTOTHERADIUS_API FPRSplineData {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FPRCleanTransform SplineShift;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FPRCleanTransform HandShiftRelatedSpline;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FSplinePoint> Points;
    
    FPRSplineData();
};

