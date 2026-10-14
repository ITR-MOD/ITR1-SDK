#pragma once
#include "CoreMinimal.h"
#include "PRCondition_Mission.h"
#include "PRCondition_MissionActive.generated.h"

UCLASS(Blueprintable, EditInlineNew)
class INTOTHERADIUS_API UPRCondition_MissionActive : public UPRCondition_Mission {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool InProgress;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool InverseCheck;
    
    UPRCondition_MissionActive();

};

