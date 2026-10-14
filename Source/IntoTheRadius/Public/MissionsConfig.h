#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "IAsset.h"
#include "IPreparable.h"
#include "MissionsCategory.h"
#include "MissionsConfig.generated.h"

class UMissionNormalConfig;
class UMissionsConfig;

UCLASS(Blueprintable)
class INTOTHERADIUS_API UMissionsConfig : public UDataAsset, public IIAsset, public IIPreparable {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    UMissionNormalConfig* TutorialMissionConfig;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FMissionsCategory> Missions;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    int32 MaxExecuting;
    
    UMissionsConfig();

    UFUNCTION(BlueprintCallable, BlueprintPure)
    static UMissionsConfig* GetMissionsConfig();
    

    // Fix for true pure virtual functions not being implemented
};

