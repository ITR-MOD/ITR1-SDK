#pragma once
#include "CoreMinimal.h"
#include "AnomalyData.h"
#include "ItemGameData.h"
#include "SpawnGameData.h"
#include "AnomalyFieldData.generated.h"

USTRUCT(Atomic, BlueprintType)
struct INTOTHERADIUS_API FAnomalyFieldData {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FAnomalyData> AnomalyData;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FSpawnGameData> ArtifactData;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FItemGameData> ItemData;
    
    FAnomalyFieldData();
};

