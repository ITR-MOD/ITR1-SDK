#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PRCleanTransform.generated.h"

USTRUCT(Atomic, BlueprintType)
struct INTOTHERADIUS_API FPRCleanTransform {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FVector Location;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FVector Rotation;
    
    FPRCleanTransform();
};

