#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ShrapnelComponentApplyDamageDelegate.generated.h"

UDELEGATE(BlueprintCallable) DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FShrapnelComponentApplyDamage, const FVector&, Location, const bool, IsPearce);

