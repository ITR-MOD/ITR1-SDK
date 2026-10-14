#pragma once
#include "CoreMinimal.h"
#include "NpcPointQueryResult.h"
#include "PointQueryCompleteDelegateDelegate.generated.h"

UDELEGATE(BlueprintCallable) DECLARE_DYNAMIC_DELEGATE_OneParam(FPointQueryCompleteDelegate, const FNpcPointQueryResult&, QueryResult);

