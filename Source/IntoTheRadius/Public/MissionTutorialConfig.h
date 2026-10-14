#pragma once
#include "CoreMinimal.h"
#include "MissionBaseConfig.h"
#include "MissionTutorialConfig.generated.h"

class UMissionTutorialObjectiveConfig;

UCLASS(Blueprintable)
class INTOTHERADIUS_API UMissionTutorialConfig : public UMissionBaseConfig {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool AutoProgress;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    UMissionTutorialObjectiveConfig* TutorialObjective;
    
    UMissionTutorialConfig();

};

