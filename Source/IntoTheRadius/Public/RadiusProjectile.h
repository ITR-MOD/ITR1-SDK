#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IPooledActor.h"
#include "IRadiusProjectile.h"
#include "RadiusProjectile.generated.h"

class URadiusProjectileInfo;

UCLASS(Blueprintable)
class INTOTHERADIUS_API ARadiusProjectile : public AActor, public IIRadiusProjectile, public IIPooledActor {
    GENERATED_BODY()
public:
protected:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float AutoDisposeTime;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float DamageDisposeTime;
    
public:
    ARadiusProjectile(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable)
    void Dispose();
    

    // Fix for true pure virtual functions not being implemented
    UFUNCTION(BlueprintCallable)
    void Launch() override PURE_VIRTUAL(Launch,);
    
    UFUNCTION(BlueprintCallable)
    void Initialize(URadiusProjectileInfo* ProjectileInfo) override PURE_VIRTUAL(Initialize,);
    
};

