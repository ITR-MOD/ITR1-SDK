#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IPooledActor.h"
#include "RadiusPooledActor.generated.h"

class UPoolDisposeComponent;

UCLASS(Blueprintable)
class INTOTHERADIUS_API ARadiusPooledActor : public AActor, public IIPooledActor {
    GENERATED_BODY()
public:
protected:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    UPoolDisposeComponent* PoolDisposeComponent;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool bExcludedFromPool;
    
public:
    ARadiusPooledActor(const FObjectInitializer& ObjectInitializer);


    // Fix for true pure virtual functions not being implemented
};

