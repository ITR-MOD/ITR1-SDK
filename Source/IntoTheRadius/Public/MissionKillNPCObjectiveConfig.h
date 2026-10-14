#pragma once
#include "CoreMinimal.h"
#include "MissionBaseObjectiveConfig.h"
#include "TagToTagsList.h"
#include "MissionKillNPCObjectiveConfig.generated.h"

UCLASS(Blueprintable, EditInlineNew)
class INTOTHERADIUS_API UMissionKillNPCObjectiveConfig : public UMissionBaseObjectiveConfig {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FTagToTagsList> PointsToNPCs;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float SpawnNPCsRadius;
    
    UMissionKillNPCObjectiveConfig();

};

