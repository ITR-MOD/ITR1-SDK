#pragma once
#include "CoreMinimal.h"
#include "PRCondition.h"
#include "PRCondition_ORCheck.generated.h"

UCLASS(Blueprintable, EditInlineNew)
class INTOTHERADIUS_API UPRCondition_ORCheck : public UPRCondition {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    TArray<UPRCondition*> ORConditions;
    
    UPRCondition_ORCheck();

};

