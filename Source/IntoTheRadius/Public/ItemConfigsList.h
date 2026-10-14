#pragma once
#include "CoreMinimal.h"
#include "ItemConfigsList.generated.h"

class UPRItemConfig;

USTRUCT(Atomic, BlueprintType)
struct INTOTHERADIUS_API FItemConfigsList {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    TArray<UPRItemConfig*> Configs;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FString CustomUID;
    
    FItemConfigsList();
};

