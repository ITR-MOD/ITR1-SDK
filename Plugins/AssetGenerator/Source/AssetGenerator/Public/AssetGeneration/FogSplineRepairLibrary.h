#pragma once
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FogSplineRepairLibrary.generated.h"
class UBlueprint;
UCLASS()
class ASSETGENERATOR_API UFogSplineRepairLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category="Asset Generator")
    static bool RepairResampleSpline(UBlueprint* Blueprint);
};
