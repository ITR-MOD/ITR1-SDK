#pragma once
#include "CoreMinimal.h"
#include "PRBasicActor.h"
#include "SubtitlesControllerInterface.h"
#include "SubtitlesController.generated.h"

UCLASS(Abstract, Blueprintable)
class INTOTHERADIUS_API ASubtitlesController : public APRBasicActor, public ISubtitlesControllerInterface {
    GENERATED_BODY()
public:
    ASubtitlesController(const FObjectInitializer& ObjectInitializer);


    // Fix for true pure virtual functions not being implemented
};

