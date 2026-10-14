#pragma once
#include "CoreMinimal.h"
#include "OnVignetteChangedDelegate.generated.h"

UDELEGATE(BlueprintCallable) DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVignetteChanged, bool, bIsOn);

