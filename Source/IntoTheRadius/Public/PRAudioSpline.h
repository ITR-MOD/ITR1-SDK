#pragma once
#include "CoreMinimal.h"
#include "PRBasicActor.h"
#include "PRAudioSpline.generated.h"

class UAudioComponent;
class USplineComponent;

UCLASS(Blueprintable)
class INTOTHERADIUS_API APRAudioSpline : public APRBasicActor {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    USplineComponent* Spline;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    UAudioComponent* Audio;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float LoopBeginMin;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float LoopBeginMax;
    
    APRAudioSpline(const FObjectInitializer& ObjectInitializer);

};

