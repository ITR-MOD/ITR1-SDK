#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ItemDamagedDelegateDelegate.generated.h"

class AActor;
class UDamageType;

UDELEGATE(BlueprintCallable) DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FItemDamagedDelegate, AActor*, causer, float, Damage, FGameplayTag, weaponTag, const UDamageType*, DamageType);

