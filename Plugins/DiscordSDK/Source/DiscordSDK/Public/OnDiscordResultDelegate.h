#pragma once
#include "CoreMinimal.h"
#include "EDiscordReturnResult.h"
#include "OnDiscordResultDelegate.generated.h"

UDELEGATE(BlueprintCallable) DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDiscordResult, EDiscordReturnResult, StateSetResult);

