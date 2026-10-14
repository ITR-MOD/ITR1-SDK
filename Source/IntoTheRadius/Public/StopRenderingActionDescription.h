#pragma once
#include "CoreMinimal.h"
#include "RelevanceActionDescription.h"
#include "StopRenderingActionDescription.generated.h"

UCLASS(Blueprintable)
class UStopRenderingActionDescription : public URelevanceActionDescription {
    GENERATED_BODY()
public:
    UStopRenderingActionDescription();

};

