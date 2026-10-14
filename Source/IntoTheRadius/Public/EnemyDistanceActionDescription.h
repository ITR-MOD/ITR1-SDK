#pragma once
#include "CoreMinimal.h"
#include "RelevanceActionDescription.h"
#include "EnemyDistanceActionDescription.generated.h"

UCLASS(Blueprintable)
class UEnemyDistanceActionDescription : public URelevanceActionDescription {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float ActorTickIntervalMultiplier;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float AnimationTickIntervalMultiplier;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float PerceptionTickIntervalMultiplier;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float MovementTickIntervalMultiplier;
    
    UEnemyDistanceActionDescription();

};

