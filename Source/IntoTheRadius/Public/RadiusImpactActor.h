#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IPooledActor.h"
#include "RadiusImpactInfo.h"
#include "RadiusImpactActor.generated.h"

class UPoolDisposeComponent;

UCLASS(Blueprintable)
class INTOTHERADIUS_API ARadiusImpactActor : public AActor, public IIPooledActor {
    GENERATED_BODY()
public:
protected:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    UPoolDisposeComponent* PoolDisposeComponent;
    
public:
    ARadiusImpactActor(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void Setup(const FRadiusImpactInfo& Info);
    

    // Fix for true pure virtual functions not being implemented
};

