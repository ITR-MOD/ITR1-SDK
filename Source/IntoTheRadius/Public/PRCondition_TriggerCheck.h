#pragma once
#include "CoreMinimal.h"
#include "PRCondition_Trigger.h"
#include "PRCondition_TriggerCheck.generated.h"

UCLASS(Blueprintable, EditInlineNew)
class INTOTHERADIUS_API UPRCondition_TriggerCheck : public UPRCondition_Trigger {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool PassedCheck;
    
    UPRCondition_TriggerCheck();

};

