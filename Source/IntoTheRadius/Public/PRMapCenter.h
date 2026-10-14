#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "PRBasicActor.h"
#include "PRMapCenter.generated.h"

UCLASS(Blueprintable)
class INTOTHERADIUS_API APRMapCenter : public APRBasicActor {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FGameplayTag LevelTag;
    
    APRMapCenter(const FObjectInitializer& ObjectInitializer);

};

