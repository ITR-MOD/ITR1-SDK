#pragma once
#include "CoreMinimal.h"
#include "PRBasicActor.h"
#include "PRPointOfInterest.generated.h"

UCLASS(Blueprintable)
class INTOTHERADIUS_API APRPointOfInterest : public APRBasicActor {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool bPoiValid;
    
    APRPointOfInterest(const FObjectInitializer& ObjectInitializer);

};

