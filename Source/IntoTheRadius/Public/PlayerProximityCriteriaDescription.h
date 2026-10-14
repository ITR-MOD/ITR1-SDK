#pragma once
#include "CoreMinimal.h"
#include "RelevanceCriteriaDescription.h"
#include "PlayerProximityCriteriaDescription.generated.h"

UCLASS(Blueprintable)
class UPlayerProximityCriteriaDescription : public URelevanceCriteriaDescription {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float PlayerDistancePC;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float PlayerDistanceQuest;
    
    UPlayerProximityCriteriaDescription();

};

