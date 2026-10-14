#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "TagToTagsList.generated.h"

USTRUCT(Atomic, BlueprintType)
struct INTOTHERADIUS_API FTagToTagsList {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FGameplayTag Tag;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FGameplayTag> List;
    
    FTagToTagsList();
};

