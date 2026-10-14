#pragma once
#include "CoreMinimal.h"
#include "RadiusAutomationBase.h"
#include "ShutdownAutomation.generated.h"

UCLASS(Blueprintable)
class INTOTHERADIUS_API UShutdownAutomation : public URadiusAutomationBase {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float ShutdownDelayInSec;
    
    UShutdownAutomation();

};

