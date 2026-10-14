#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "PRCondition.h"
#include "PRCondition_Mission.generated.h"

UCLASS(Abstract, Blueprintable, EditInlineNew)
class INTOTHERADIUS_API UPRCondition_Mission : public UPRCondition {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FGameplayTag MissionConfigId;
    
    UPRCondition_Mission();

};

