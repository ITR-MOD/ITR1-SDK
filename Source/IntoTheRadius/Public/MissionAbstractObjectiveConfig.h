#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MissionAbstractObjectiveConfig.generated.h"

UCLASS(Abstract, Blueprintable, EditInlineNew)
class INTOTHERADIUS_API UMissionAbstractObjectiveConfig : public UObject {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FText ObjectiveShortName;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FText ObjectiveLongName;
    
    UMissionAbstractObjectiveConfig();

};

