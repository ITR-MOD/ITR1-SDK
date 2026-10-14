#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "NPCCloseCombatConfig.generated.h"

USTRUCT(BlueprintType)
struct INTOTHERADIUS_API FNPCCloseCombatConfig : public FTableRowBase {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool UseCloseCombatManager;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float CloseCombatWeight;
    
    FNPCCloseCombatConfig();
};

