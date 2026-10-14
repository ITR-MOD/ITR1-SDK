#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MissionAbstractObjectiveData.h"
#include "MissionRecord.h"
#include "TagToTagsList.h"
#include "MissionTakePictureObjectiveData.generated.h"

UCLASS(Blueprintable)
class INTOTHERADIUS_API UMissionTakePictureObjectiveData : public UMissionAbstractObjectiveData {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FTagToTagsList> PointsToObjects;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FMissionRecord> CollectedPhotos;
    
    UMissionTakePictureObjectiveData();

    UFUNCTION(BlueprintCallable, BlueprintPure)
    TArray<FGameplayTag> GetObjectsTags();
    
};

