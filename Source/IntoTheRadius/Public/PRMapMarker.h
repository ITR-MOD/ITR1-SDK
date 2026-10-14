#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "EMapMarker.h"
#include "PRBasicActor.h"
#include "PRMapMarker.generated.h"

class UPRCondition;

UCLASS(Abstract, Blueprintable)
class INTOTHERADIUS_API APRMapMarker : public APRBasicActor {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FGameplayTag MarkerID;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    EMapMarker MarkerType;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    TArray<UPRCondition*> Conditions;
    
    APRMapMarker(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsConditionsPassed();
    
};

