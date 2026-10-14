#pragma once
#include "CoreMinimal.h"
#include "PRBasicActor.h"
#include "RadiusTeleportArcController.generated.h"

UCLASS(Blueprintable)
class INTOTHERADIUS_API ARadiusTeleportArcController : public APRBasicActor {
    GENERATED_BODY()
public:
    ARadiusTeleportArcController(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void TickArc(const float DeltaSeconds);
    
};

