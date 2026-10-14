#pragma once
#include "CoreMinimal.h"
#include "MapData.generated.h"

USTRUCT(Atomic, BlueprintType)
struct INTOTHERADIUS_API FMapData {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<uint8> Data;
    
    FMapData();
};

