#pragma once
#include "CoreMinimal.h"
#include "OneParamBoolDelegate.generated.h"

UDELEGATE(BlueprintCallable) DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOneParamBool, bool, Value);

