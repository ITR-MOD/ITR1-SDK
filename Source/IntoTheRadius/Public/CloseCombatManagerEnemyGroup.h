#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "CloseCombatManagerEnemyGroup.generated.h"

USTRUCT(BlueprintType)
struct INTOTHERADIUS_API FCloseCombatManagerEnemyGroup {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    int32 MinEnemyAllowed;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FGameplayTagContainer EnemyGroup;
    
    FCloseCombatManagerEnemyGroup();
};

