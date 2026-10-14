#pragma once
#include "CoreMinimal.h"
#include "ConditionsList.generated.h"

class UPRCondition;

USTRUCT(Atomic, BlueprintType)
struct INTOTHERADIUS_API FConditionsList {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    TArray<UPRCondition*> Conditions;
    
    FConditionsList();
};

