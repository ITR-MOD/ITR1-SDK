#pragma once
#include "CoreMinimal.h"
#include "PRCondition_IntCheck.h"
#include "PRCondition_SuccesfullyFinishedCountCheck.generated.h"

UCLASS(Blueprintable, EditInlineNew)
class INTOTHERADIUS_API UPRCondition_SuccesfullyFinishedCountCheck : public UPRCondition_IntCheck {
    GENERATED_BODY()
public:
    UPRCondition_SuccesfullyFinishedCountCheck();

};

