#pragma once
#include "CoreMinimal.h"
#include "AITypes.h"
#include "Navigation/PathFollowingComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "UObject/Object.h"
#include "PlayFootstepHelper.generated.h"

UCLASS(Blueprintable)
class UPlayFootstepHelper : public UObject {
    GENERATED_BODY()
public:
    UPlayFootstepHelper();

    UFUNCTION(BlueprintCallable)
    void OnMoveStart(FAIRequestID RequestID, TEnumAsByte<EPathFollowingStatus::Type> MovementStatus);
    
    UFUNCTION(BlueprintCallable)
    void OnMoveEnd(FAIRequestID RequestID, TEnumAsByte<EPathFollowingResult::Type> MovementResult);
    
};

