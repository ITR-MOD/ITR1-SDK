#pragma once
#include "CoreMinimal.h"
#include "PRCondition.h"
#include "PRCondition_MORCheck.generated.h"

UCLASS(Blueprintable, EditInlineNew)
class INTOTHERADIUS_API UPRCondition_MORCheck : public UPRCondition {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    int32 Minimum;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    TArray<UPRCondition*> Conditions;
    
    UPRCondition_MORCheck();

};

