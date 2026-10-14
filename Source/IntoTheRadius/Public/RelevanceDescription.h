#pragma once
#include "CoreMinimal.h"
#include "RelevanceDescription.generated.h"

class URelevanceActionDescription;
class URelevanceCriteriaDescription;

USTRUCT(BlueprintType)
struct INTOTHERADIUS_API FRelevanceDescription {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    URelevanceCriteriaDescription* Criteria;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<URelevanceActionDescription*> Actions;
    
    FRelevanceDescription();
};

