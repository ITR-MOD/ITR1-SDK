#pragma once
#include "CoreMinimal.h"
#include "EPRControllerType.h"
#include "PRCleanTransform.h"
#include "PRHandPoseConditions.h"
#include "PRSplineData.h"
#include "PRHandPoseInfo.generated.h"

USTRUCT(BlueprintType)
struct INTOTHERADIUS_API FPRHandPoseInfo {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<EPRControllerType, FPRCleanTransform> ItemShiftPerController;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FPRCleanTransform HandShift;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FPRCleanTransform PointShift;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FPRSplineData SplineData;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FPRHandPoseConditions HandPoseConditions;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FPRCleanTransform FingerThumbShift;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FPRCleanTransform FingerIndexShift;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FPRCleanTransform FingerMiddleShift;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FPRCleanTransform FingerRingShift;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FPRCleanTransform FingerPinkyShift;
    
    FPRHandPoseInfo();
};

