#pragma once
#include "CoreMinimal.h"
#include "NameArray.generated.h"

USTRUCT(Atomic, BlueprintType)
struct INTOTHERADIUS_API FNameArray {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FName> Names;
    
    FNameArray();
};

