#pragma once
#include "CoreMinimal.h"
#include "PRCondition_FloatCheck.h"
#include "PRCondition_MoneyCheck.generated.h"

UCLASS(Blueprintable, EditInlineNew)
class INTOTHERADIUS_API UPRCondition_MoneyCheck : public UPRCondition_FloatCheck {
    GENERATED_BODY()
public:
    UPRCondition_MoneyCheck();

};

