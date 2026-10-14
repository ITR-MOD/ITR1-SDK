#pragma once
#include "CoreMinimal.h"
#include "RelevanceCriteriaDescription.h"
#include "PlayerViewportCriteriaDescription.generated.h"

UCLASS(Blueprintable)
class UPlayerViewportCriteriaDescription : public URelevanceCriteriaDescription {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool CheckBoundingBox;
    
    UPlayerViewportCriteriaDescription();

};

