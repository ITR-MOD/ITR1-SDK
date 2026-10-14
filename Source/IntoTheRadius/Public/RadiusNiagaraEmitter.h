#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IPooledActor.h"
#include "RadiusNiagaraEmitter.generated.h"

class UNiagaraComponent;
class UPoolDisposeComponent;

UCLASS(Blueprintable)
class INTOTHERADIUS_API ARadiusNiagaraEmitter : public AActor, public IIPooledActor {
    GENERATED_BODY()
public:
protected:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    UPoolDisposeComponent* PoolDisposeComponent;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    UNiagaraComponent* ParticleSystemComponent;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float DisposeTime;
    
public:
    ARadiusNiagaraEmitter(const FObjectInitializer& ObjectInitializer);


    // Fix for true pure virtual functions not being implemented
};

