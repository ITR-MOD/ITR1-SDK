#pragma once
#include "CoreMinimal.h"
#include "MissionBaseHintObjectiveConfig.h"
#include "TagToItemConfigsList.h"
#include "MissionFindStashObjectiveConfig.generated.h"

UCLASS(Blueprintable, EditInlineNew)
class INTOTHERADIUS_API UMissionFindStashObjectiveConfig : public UMissionBaseHintObjectiveConfig {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FTagToItemConfigsList> PointsToStashes;
    
    UMissionFindStashObjectiveConfig();

};

