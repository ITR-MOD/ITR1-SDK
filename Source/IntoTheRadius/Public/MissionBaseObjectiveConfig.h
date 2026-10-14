#pragma once
#include "CoreMinimal.h"
#include "MissionAbstractObjectiveConfig.h"
#include "MissionBaseObjectiveConfig.generated.h"

class UPRModifiableInt;

UCLASS(Abstract, Blueprintable, EditInlineNew)
class INTOTHERADIUS_API UMissionBaseObjectiveConfig : public UMissionAbstractObjectiveConfig {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    UPRModifiableInt* RewardMoneyOverride;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    UPRModifiableInt* RewardPlayerLevelOverride;
    
    UMissionBaseObjectiveConfig();

};

