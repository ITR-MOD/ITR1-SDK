#pragma once
#include "CoreMinimal.h"
#include "MinMaxInt.h"
#include "PRCondition.h"
#include "PRCondition_PlayerLevelCheck.generated.h"

UCLASS(Blueprintable, EditInlineNew)
class INTOTHERADIUS_API UPRCondition_PlayerLevelCheck : public UPRCondition {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FMinMaxInt PlayerLevelRange;
    
    UPRCondition_PlayerLevelCheck();

};

