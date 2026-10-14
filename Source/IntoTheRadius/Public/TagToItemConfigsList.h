#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "TagToItemConfigsList.generated.h"

class UPRItemConfig;

USTRUCT(Atomic, BlueprintType)
struct INTOTHERADIUS_API FTagToItemConfigsList {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FGameplayTag Tag;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    TArray<UPRItemConfig*> Configs;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FString CustomUID;
    
    FTagToItemConfigsList();
};

