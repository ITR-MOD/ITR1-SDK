#pragma once
#include "CoreMinimal.h"
#include "ItemGameData.h"
#include "SpawnSlotData.generated.h"

USTRUCT(Atomic, BlueprintType)
struct INTOTHERADIUS_API FSpawnSlotData {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool IsEmpty;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool IsSpawned;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FItemGameData> ItemsData;
    
    FSpawnSlotData();
};

