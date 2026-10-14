#pragma once
#include "CoreMinimal.h"
#include "MinMaxInt.h"
#include "PRCondition_Mission.h"
#include "PRCondition_MissionsFinishedAfter.generated.h"

UCLASS(Blueprintable, EditInlineNew)
class INTOTHERADIUS_API UPRCondition_MissionsFinishedAfter : public UPRCondition_Mission {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool OnlySuccessful;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FMinMaxInt Amount;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool InverseCheck;
    
    UPRCondition_MissionsFinishedAfter();

};

