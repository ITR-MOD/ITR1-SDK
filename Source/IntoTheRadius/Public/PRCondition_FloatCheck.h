#pragma once
#include "CoreMinimal.h"
#include "PRCondition.h"
#include "PRCondition_FloatCheck.generated.h"

class UMathCondition_Float;

UCLASS(Abstract, Blueprintable, EditInlineNew)
class INTOTHERADIUS_API UPRCondition_FloatCheck : public UPRCondition {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    UMathCondition_Float* ValueCheck;
    
    UPRCondition_FloatCheck();

};

