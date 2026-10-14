#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RelevanceCriteriaDescription.generated.h"

UCLASS(Abstract, Blueprintable)
class URelevanceCriteriaDescription : public UDataAsset {
    GENERATED_BODY()
public:
    URelevanceCriteriaDescription();

};

