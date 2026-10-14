#pragma once
#include "CoreMinimal.h"
#include "NPCGameData.h"
#include "NPCDataMap.generated.h"

USTRUCT(Atomic, BlueprintType)
struct INTOTHERADIUS_API FNPCDataMap {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<FString, FNPCGameData> DataMap;
    
    FNPCDataMap();
};

