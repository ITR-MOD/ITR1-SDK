#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "RadiusAutomationBase.h"
#include "TeleportAutomation.generated.h"

class AActor;

UCLASS(Blueprintable)
class INTOTHERADIUS_API UTeleportAutomation : public URadiusAutomationBase {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float TeleportDelayInSec;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    int32 MaxLevelPosition;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FGameplayTag> LevelList;
    
private:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<AActor*> LevelPositionList;
    
public:
    UTeleportAutomation();

private:
    UFUNCTION(BlueprintCallable)
    void LevelReadyCallback();
    
};

