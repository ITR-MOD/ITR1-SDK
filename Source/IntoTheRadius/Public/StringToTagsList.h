#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "StringToTagsList.generated.h"

USTRUCT(Atomic, BlueprintType)
struct INTOTHERADIUS_API FStringToTagsList {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FString Key;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FGameplayTag> List;
    
    FStringToTagsList();
};

