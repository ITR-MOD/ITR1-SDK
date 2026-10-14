#pragma once
#include "CoreMinimal.h"
#include "Perception/AIPerceptionComponent.h"
#include "AIEdPerceptionComp.generated.h"

UCLASS(Blueprintable, ClassGroup=Custom, meta=(BlueprintSpawnableComponent))
class INTOTHERADIUS_API UAIEdPerceptionComp : public UAIPerceptionComponent {
    GENERATED_BODY()
public:
    UAIEdPerceptionComp(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable)
    bool SetSightHalfAngle(float HalfAngle);
    
    UFUNCTION(BlueprintCallable)
    bool SetSightDistance(float Distance);
    
    UFUNCTION(BlueprintCallable)
    bool SetHearingDistance(float Distance);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    float GetSightHalfAngle();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    float GetSightDistance();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    float GetHearingDistance();
    
};

