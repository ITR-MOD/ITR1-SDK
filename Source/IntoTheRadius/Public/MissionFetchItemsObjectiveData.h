#pragma once
#include "CoreMinimal.h"
#include "MissionAbstractObjectiveData.h"
#include "MissionRecord.h"
#include "TagToItemConfigsList.h"
#include "TagToTagsList.h"
#include "MissionFetchItemsObjectiveData.generated.h"

UCLASS(Blueprintable)
class INTOTHERADIUS_API UMissionFetchItemsObjectiveData : public UMissionAbstractObjectiveData {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FTagToTagsList> PointsToDefaultItems;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FTagToItemConfigsList> PointsToConfigItems;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FMissionRecord> CollectedItems;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool SpawnItems;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool CleanItems;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float SpawnRadiusForOnePoint;
    
    UMissionFetchItemsObjectiveData();

};

