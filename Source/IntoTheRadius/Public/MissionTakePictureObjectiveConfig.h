#pragma once
#include "CoreMinimal.h"
#include "MissionBaseObjectiveConfig.h"
#include "TagToTagsList.h"
#include "MissionTakePictureObjectiveConfig.generated.h"

UCLASS(Blueprintable, EditInlineNew)
class INTOTHERADIUS_API UMissionTakePictureObjectiveConfig : public UMissionBaseObjectiveConfig {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FTagToTagsList> PointsToObjects;
    
    UMissionTakePictureObjectiveConfig();

};

