#include "PRItemDurabilityComponent.h"

UPRItemDurabilityComponent::UPRItemDurabilityComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
}

void UPRItemDurabilityComponent::OnTakePointDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* HitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser) {
}

void UPRItemDurabilityComponent::OnTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser) {
}

bool UPRItemDurabilityComponent::ChangeDurability(float Damage, AActor* causer, FGameplayTag weponTag, const UDamageType* DamageType) {
    return false;
}


