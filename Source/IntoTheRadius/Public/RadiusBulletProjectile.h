#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/NoExportTypes.h"
#include "Engine/EngineTypes.h"
#include "RadiusProjectile.h"
#include "RadiusBulletProjectile.generated.h"

class AActor;
class UPrimitiveComponent;
class URadiusBulletComponent;
class URadiusBulletImpulseComponent;
class URadiusBulletTracerComponent;
class USphereComponent;

UCLASS(Blueprintable)
class INTOTHERADIUS_API ARadiusBulletProjectile : public ARadiusProjectile {
    GENERATED_BODY()
public:
protected:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float BaseDamage;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    USphereComponent* SphereCollider;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    URadiusBulletTracerComponent* BulletTracerComponent;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    URadiusBulletComponent* BulletComponent;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    URadiusBulletImpulseComponent* BulletImpulseComponent;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool DebugMode;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FVector2D ParticalScaleInputRange;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FVector2D ParticalScaleOutputRange;
    
public:
    ARadiusBulletProjectile(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable)
    void SphereColliderHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnSpawnImpact(const FHitResult& Hit, const float Scale);
    
    UFUNCTION(BlueprintCallable)
    void BulletComponentRicochet(const FVector& Location, const FVector& Force, const bool IsPenetration);
    
    UFUNCTION(BlueprintCallable)
    void BulletComponentImpact(const FHitResult& Hit);
    
};

