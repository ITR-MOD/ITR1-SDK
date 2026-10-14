#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ShiftsPair.generated.h"

USTRUCT(Atomic, BlueprintType)
struct INTOTHERADIUS_API FShiftsPair {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FTransform LeftShift;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FTransform RightShift;
    
    FShiftsPair();
};

