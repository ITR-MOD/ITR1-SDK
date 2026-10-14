#pragma once
#include "CoreMinimal.h"
#include "MissionAbstractObjectiveConfig.h"
#include "MissionBaseHintObjectiveConfig.generated.h"

UCLASS(Abstract, Blueprintable, EditInlineNew)
class INTOTHERADIUS_API UMissionBaseHintObjectiveConfig : public UMissionAbstractObjectiveConfig {
    GENERATED_BODY()
public:
    UMissionBaseHintObjectiveConfig();

};

