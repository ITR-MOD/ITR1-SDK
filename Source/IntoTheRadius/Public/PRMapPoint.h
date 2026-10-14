#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "PRBasicActor.h"
#include "PRMapPoint.generated.h"

UCLASS(Blueprintable)
class INTOTHERADIUS_API APRMapPoint : public APRBasicActor {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FGameplayTag pointTag;
    
    APRMapPoint(const FObjectInitializer& ObjectInitializer);

};

