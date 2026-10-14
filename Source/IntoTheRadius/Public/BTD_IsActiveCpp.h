#pragma once
#include "CoreMinimal.h"
#include "BTD_BlackboardBool.h"
#include "BTD_IsActiveCpp.generated.h"

class AActor;

UCLASS(Blueprintable)
class INTOTHERADIUS_API UBTD_IsActiveCpp : public UBTD_BlackboardBool {
    GENERATED_BODY()
public:
    UBTD_IsActiveCpp();

    UFUNCTION(BlueprintCallable)
    void OnNPCDied(AActor* DeadNPC);
    
};

