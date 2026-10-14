#pragma once
#include "CoreMinimal.h"
#include "OneParamEnvironmentDelegate.generated.h"

class APREnvironment;

UDELEGATE(BlueprintCallable) DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOneParamEnvironment, const APREnvironment*, Environment);

