#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "NPCAwarenessConfig.generated.h"

USTRUCT(BlueprintType)
struct INTOTHERADIUS_API FNPCAwarenessConfig : public FTableRowBase {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float AwarenessLowLevel;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float AwarenessHighLevel;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float AwarenessDecrease;
    
    FNPCAwarenessConfig();
};

