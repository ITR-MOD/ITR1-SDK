#pragma once
#include "CoreMinimal.h"
#include "PRCondition_Mission.h"
#include "PRCondition_HoursSinceMissionSuccessfullyFinished.generated.h"

class UMathCondition_Int;

UCLASS(Blueprintable, EditInlineNew)
class INTOTHERADIUS_API UPRCondition_HoursSinceMissionSuccessfullyFinished : public UPRCondition_Mission {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    UMathCondition_Int* ValueCheck;
    
    UPRCondition_HoursSinceMissionSuccessfullyFinished();

};

