#pragma once
#include "CoreMinimal.h"
#include "RelevanceActionDescription.h"
#include "AlterTickActionDescription.generated.h"

UCLASS(Blueprintable)
class UAlterTickActionDescription : public URelevanceActionDescription {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool StopTick;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float TickIntervalMultiplier;
    
    UAlterTickActionDescription();

};

