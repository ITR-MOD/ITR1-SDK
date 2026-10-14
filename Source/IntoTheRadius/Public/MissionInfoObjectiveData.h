#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MissionAbstractObjectiveData.h"
#include "MissionInfoObjectiveData.generated.h"

UCLASS(Blueprintable)
class INTOTHERADIUS_API UMissionInfoObjectiveData : public UMissionAbstractObjectiveData {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FGameplayTag> Points;
    
    UMissionInfoObjectiveData();

};

