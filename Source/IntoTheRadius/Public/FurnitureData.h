#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FurnitureData.generated.h"

USTRUCT(Atomic, BlueprintType)
struct INTOTHERADIUS_API FFurnitureData {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<FName, FTransform> SocketTransform;
    
    FFurnitureData();
};

