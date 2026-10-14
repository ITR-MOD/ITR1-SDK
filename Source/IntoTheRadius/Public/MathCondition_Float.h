#pragma once
#include "CoreMinimal.h"
#include "MathCondition.h"
#include "MathCondition_Float.generated.h"

UCLASS(Abstract, Blueprintable, EditInlineNew)
class INTOTHERADIUS_API UMathCondition_Float : public UMathCondition {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float Value;
    
    UMathCondition_Float();

};

