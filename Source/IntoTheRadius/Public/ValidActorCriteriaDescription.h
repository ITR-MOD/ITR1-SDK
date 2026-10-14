#pragma once
#include "CoreMinimal.h"
#include "RelevanceCriteriaDescription.h"
#include "ValidActorCriteriaDescription.generated.h"

UCLASS(Blueprintable)
class UValidActorCriteriaDescription : public URelevanceCriteriaDescription {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool bAlwaysOffForPC;
    
    UValidActorCriteriaDescription();

};

