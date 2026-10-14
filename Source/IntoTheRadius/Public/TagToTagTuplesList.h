#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsTuple.h"
#include "TagToTagTuplesList.generated.h"

USTRUCT(Atomic, BlueprintType)
struct INTOTHERADIUS_API FTagToTagTuplesList {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FGameplayTag Tag;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FGameplayTagsTuple> List;
    
    FTagToTagTuplesList();
};

