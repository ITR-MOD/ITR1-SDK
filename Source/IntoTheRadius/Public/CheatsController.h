#pragma once
#include "CoreMinimal.h"
#include "CheatsControllerInterface.h"
#include "PRBasicActor.h"
#include "CheatsController.generated.h"

UCLASS(Abstract, Blueprintable)
class INTOTHERADIUS_API ACheatsController : public APRBasicActor, public ICheatsControllerInterface {
    GENERATED_BODY()
public:
    ACheatsController(const FObjectInitializer& ObjectInitializer);


    // Fix for true pure virtual functions not being implemented
};

