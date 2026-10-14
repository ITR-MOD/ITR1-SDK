#pragma once
#include "CoreMinimal.h"
#include "PRLootTableConfig.generated.h"

class UDataTable;
class UPRCondition;

USTRUCT(Atomic, BlueprintType)
struct INTOTHERADIUS_API FPRLootTableConfig {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    TArray<UPRCondition*> Conditions;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    int32 LootPoints;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    UDataTable* LootTable;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool ExcludeGeneration;
    
    FPRLootTableConfig();
};

