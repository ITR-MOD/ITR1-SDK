#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "OneParamTagDelegate.generated.h"

UDELEGATE(BlueprintCallable) DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOneParamTag, FGameplayTag, Value);

