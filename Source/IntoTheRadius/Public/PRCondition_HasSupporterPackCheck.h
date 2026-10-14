#pragma once
#include "CoreMinimal.h"
#include "PRCondition.h"
#include "PRCondition_HasSupporterPackCheck.generated.h"

UCLASS(Blueprintable, EditInlineNew)
class INTOTHERADIUS_API UPRCondition_HasSupporterPackCheck : public UPRCondition {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool bValue;
    
    UPRCondition_HasSupporterPackCheck();

};

