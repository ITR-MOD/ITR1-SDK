#pragma once
#include "CoreMinimal.h"
#include "PRBasicActor.h"
#include "PRClusterPattern.generated.h"

class USceneComponent;

UCLASS(Abstract, Blueprintable)
class INTOTHERADIUS_API APRClusterPattern : public APRBasicActor {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    USceneComponent* Root;
    
    APRClusterPattern(const FObjectInitializer& ObjectInitializer);

};

