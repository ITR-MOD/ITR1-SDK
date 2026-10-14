#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RelevanceActionDescription.generated.h"

UCLASS(Abstract, Blueprintable)
class URelevanceActionDescription : public UDataAsset {
    GENERATED_BODY()
public:
    URelevanceActionDescription();

};

