#pragma once
#include "CoreMinimal.h"
#include "PRCondition_Trigger.h"
#include "PRCondition_CassetteCheck.generated.h"

UCLASS(Blueprintable, EditInlineNew)
class INTOTHERADIUS_API UPRCondition_CassetteCheck : public UPRCondition_Trigger {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool PassedCheck;
    
    UPRCondition_CassetteCheck();

};

