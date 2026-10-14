#pragma once
#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_IsAtLocation.h"
#include "BTD_DistanceToCpp.generated.h"

UCLASS(Blueprintable)
class INTOTHERADIUS_API UBTD_DistanceToCpp : public UBTDecorator_IsAtLocation {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float RefreshInterval;
    
    UBTD_DistanceToCpp();

};

