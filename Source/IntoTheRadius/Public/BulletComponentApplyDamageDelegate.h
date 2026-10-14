#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BulletComponentApplyDamageDelegate.generated.h"

UDELEGATE(BlueprintCallable) DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBulletComponentApplyDamage, const FVector&, Location, const bool, IsPearce);

