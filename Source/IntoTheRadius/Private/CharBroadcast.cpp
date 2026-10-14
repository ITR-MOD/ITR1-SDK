#include "CharBroadcast.h"
#include "Components/SphereComponent.h"

ACharBroadcast::ACharBroadcast(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->RootComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Broadcast Sphere"));
    this->Sphere = (USphereComponent*)RootComponent;
    this->Radius = 1500.00f;
    this->LifeTime = 1.00f;
}

ACharBroadcast* ACharBroadcast::SpawnBroadcast(UObject* WorldContext, FTransform SpawnTransform, FGameplayTag MissionKillPointTag, float LifeSpan, float SphereRadius) {
    return NULL;
}

void ACharBroadcast::AlertOthers(AActor* Caller, AActor*& Target, FTransform Location) {
}


