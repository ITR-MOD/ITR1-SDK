#pragma once
#include "CoreMinimal.h"
#include "ItemConfigsList.h"
#include "SpawnGameData.h"
#include "StashData.generated.h"

USTRUCT(BlueprintType)
struct INTOTHERADIUS_API FStashData : public FSpawnGameData {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FItemConfigsList ItemConfigs;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool UseCustomConfigs;
    
    FStashData();
};

