#include "RadiusBulletProjectile.h"
#include "Components/SphereComponent.h"
#include "RadiusBulletComponent.h"
#include "RadiusBulletImpulseComponent.h"
#include "RadiusBulletTracerComponent.h"

ARadiusBulletProjectile::ARadiusBulletProjectile(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->RootComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
    this->BaseDamage = 0.00f;
    this->SphereCollider = (USphereComponent*)RootComponent;
    this->BulletTracerComponent = CreateDefaultSubobject<URadiusBulletTracerComponent>(TEXT("TraceMesh"));
    this->BulletComponent = CreateDefaultSubobject<URadiusBulletComponent>(TEXT("BulletComponent"));
    this->BulletImpulseComponent = CreateDefaultSubobject<URadiusBulletImpulseComponent>(TEXT("ImpulseComponent"));
    this->DebugMode = false;
    this->BulletTracerComponent->SetupAttachment(RootComponent);
}

void ARadiusBulletProjectile::SphereColliderHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit) {
}


void ARadiusBulletProjectile::BulletComponentRicochet(const FVector& Location, const FVector& Force, const bool IsPenetration) {
}

void ARadiusBulletProjectile::BulletComponentImpact(const FHitResult& Hit) {
}


