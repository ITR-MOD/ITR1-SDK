#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IPooledActor.h"
#include "RadiusEmitter.generated.h"

class UParticleSystemComponent;
class UPoolDisposeComponent;

UCLASS(Blueprintable)
class INTOTHERADIUS_API ARadiusEmitter : public AActor, public IIPooledActor {
    GENERATED_BODY()
public:
protected:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    UPoolDisposeComponent* PoolDisposeComponent;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    UParticleSystemComponent* ParticleSystemComponent;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FName ReflectionParamaterName;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float DisposeTime;
    
public:
    ARadiusEmitter(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable)
    UParticleSystemComponent* GetParticalSystemRef();
    

    // Fix for true pure virtual functions not being implemented
};

