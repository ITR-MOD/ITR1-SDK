#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ThreeParamsGTagIntAndStringsArrayDelegate.generated.h"

UDELEGATE(BlueprintCallable) DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FThreeParamsGTagIntAndStringsArray, FGameplayTag, Tag, int32, Value, const TArray<FString>&, StringsArray);

