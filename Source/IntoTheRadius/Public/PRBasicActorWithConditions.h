#pragma once
#include "CoreMinimal.h"
#include "PRBasicActor.h"
#include "PRBasicActorWithConditions.generated.h"

class UPRCondition;

UCLASS(Abstract, Blueprintable)
class INTOTHERADIUS_API APRBasicActorWithConditions : public APRBasicActor {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    TArray<UPRCondition*> Conditions;
    
    APRBasicActorWithConditions(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool CheckActorConditions();
    
};

