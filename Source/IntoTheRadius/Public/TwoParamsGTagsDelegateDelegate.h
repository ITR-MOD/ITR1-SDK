#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "TwoParamsGTagsDelegateDelegate.generated.h"

UDELEGATE(BlueprintCallable) DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTwoParamsGTagsDelegate, FGameplayTag, tagOne, FGameplayTag, tagTwo);

