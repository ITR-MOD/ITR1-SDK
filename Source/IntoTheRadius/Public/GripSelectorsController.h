#pragma once
#include "CoreMinimal.h"
#include "GripSelectorsControllerInterface.h"
#include "PRBasicActor.h"
#include "GripSelectorsController.generated.h"

UCLASS(Abstract, Blueprintable)
class INTOTHERADIUS_API AGripSelectorsController : public APRBasicActor, public IGripSelectorsControllerInterface {
    GENERATED_BODY()
public:
    AGripSelectorsController(const FObjectInitializer& ObjectInitializer);


    // Fix for true pure virtual functions not being implemented
};

