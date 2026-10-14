#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BulletImpulseWhistleDelegate.h"
#include "PRBasicActorComponent.h"
#include "RadiusBulletImpulseComponent.generated.h"

class UCurveFloat;
class UPrimitiveComponent;

UCLASS(Blueprintable, ClassGroup=Custom, meta=(BlueprintSpawnableComponent))
class INTOTHERADIUS_API URadiusBulletImpulseComponent : public UPRBasicActorComponent {
    GENERATED_BODY()
public:
private:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    UPrimitiveComponent* _target;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    UCurveFloat* _speedCurve;
    
protected:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float DistanceDivider;
    
public:
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FBulletImpulseWhistle OnWhistleEvent;
    
    URadiusBulletImpulseComponent(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable)
    void Suspend(bool IsSuspend);
    
    UFUNCTION(BlueprintCallable)
    void Stop();
    
    UFUNCTION(BlueprintCallable)
    void Start();
    
    UFUNCTION(BlueprintCallable)
    void Ricochet(const FVector& Location, const FVector& Force, const bool IsPenetration);
    
    UFUNCTION(BlueprintCallable)
    void Initialize(UPrimitiveComponent* TargetComponent, const float Mass, const float InitialVelocity, UCurveFloat* SpeedCurve);
    
};

