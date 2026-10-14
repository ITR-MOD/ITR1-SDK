#pragma once
#include "CoreMinimal.h"
#include "PRCondition.h"
#include "PRCondition_IntCheck.generated.h"

class UMathCondition_Int;

UCLASS(Abstract, Blueprintable, EditInlineNew)
class INTOTHERADIUS_API UPRCondition_IntCheck : public UPRCondition {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    UMathCondition_Int* ValueCheck;
    
    UPRCondition_IntCheck();

};

