#pragma once
#include "CoreMinimal.h"
#include "MissionAbstractObjectiveData.h"
#include "TagToItemConfigsList.h"
#include "MissionFindStashObjectiveData.generated.h"

UCLASS(Blueprintable)
class INTOTHERADIUS_API UMissionFindStashObjectiveData : public UMissionAbstractObjectiveData {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FTagToItemConfigsList> PointsToStashes;
    
    UMissionFindStashObjectiveData();

};

