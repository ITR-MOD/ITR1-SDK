#pragma once
#include "CoreMinimal.h"
#include "PRLootTableConfig.h"
#include "PRLootTablesByTier.generated.h"

USTRUCT(Atomic, BlueprintType)
struct INTOTHERADIUS_API FPRLootTablesByTier {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool RandomizePassedTier;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<int32, FPRLootTableConfig> Tiers;
    
    FPRLootTablesByTier();
};

