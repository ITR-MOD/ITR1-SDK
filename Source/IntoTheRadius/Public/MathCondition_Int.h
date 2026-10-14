#pragma once
#include "CoreMinimal.h"
#include "MathCondition.h"
#include "MathCondition_Int.generated.h"

UCLASS(Abstract, Blueprintable, EditInlineNew)
class INTOTHERADIUS_API UMathCondition_Int : public UMathCondition {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    int32 Value;
    
    UMathCondition_Int();

};

