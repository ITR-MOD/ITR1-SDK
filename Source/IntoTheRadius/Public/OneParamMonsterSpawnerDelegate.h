#pragma once
#include "CoreMinimal.h"
#include "OneParamMonsterSpawnerDelegate.generated.h"

class AMonsterSpawner;

UDELEGATE(BlueprintCallable) DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOneParamMonsterSpawner, AMonsterSpawner*, MonsterSpawner);

