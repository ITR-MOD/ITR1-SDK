#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/NoExportTypes.h"
#include "PRHandPoseInfo.h"
#include "IHandPoser.generated.h"

UINTERFACE(Blueprintable, MinimalAPI)
class UIHandPoser : public UInterface {
    GENERATED_BODY()
};

class IIHandPoser : public IInterface {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void GetPoseConfig(FPRHandPoseInfo& Info);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void GetItemShift(FTransform& Shift);
    
};

