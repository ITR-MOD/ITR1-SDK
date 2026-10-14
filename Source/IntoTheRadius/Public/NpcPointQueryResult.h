#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "NpcPointQueryResult.generated.h"

USTRUCT(BlueprintType)
struct FNpcPointQueryResult {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool bPointFound;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FVector Point;
    
    INTOTHERADIUS_API FNpcPointQueryResult();
};

