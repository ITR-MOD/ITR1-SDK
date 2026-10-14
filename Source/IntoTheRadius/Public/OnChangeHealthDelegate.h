#pragma once
#include "CoreMinimal.h"
#include "OnChangeHealthDelegate.generated.h"

UDELEGATE(BlueprintCallable) DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangeHealth, float, HealthDelta);

