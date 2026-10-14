#include "AliveStatsComponent.h"

UAliveStatsComponent::UAliveStatsComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->Health = 100.00f;
    this->MaxHealth = 100.00f;
    this->bRegeneration = false;
    this->MaxRegenHealth = 80.00f;
    this->RegenerationSpeed = 0.10f;
    this->RegenHealthLowLevel = 30.00f;
    this->bNoHealth = false;
    this->bAutoRegeneration = false;
    this->bAllowAutoRegeneration = false;
    this->bLowHealth = false;
    this->Stamina = 100.00f;
    this->StaminaMax = 100.00f;
    this->StaminaSpeed = 2.00f;
    this->bStaminaUse = false;
    this->bStaminaEnded = false;
    this->bLowStamina = false;
    this->OwnerActor = NULL;
    this->bIgnoreNextAnyDamage = false;
    this->LastDamageCauser = NULL;
    this->LastDamageWithSmoothing = false;
    this->LastDamageByActor = NULL;
    this->CritComponent = NULL;
    this->bPrintStats = true;
    this->bCritsOnly = false;
    this->bCanDie = true;
    this->IgnoredDamageType = NULL;
}

void UAliveStatsComponent::UseStamina(bool bUse, float UseSpeed) {
}

void UAliveStatsComponent::OnTakePointDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* HitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser) {
}

void UAliveStatsComponent::OnTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser) {
}

void UAliveStatsComponent::Kill() {
}

bool UAliveStatsComponent::IsCriticalHit(const UObject* HitComponent, FName BoneName) {
    return false;
}

bool UAliveStatsComponent::IsAlive() {
    return false;
}

void UAliveStatsComponent::HealthRegeneration() {
}

float UAliveStatsComponent::GetHealthPercent() {
    return 0.0f;
}

void UAliveStatsComponent::DisplayStats() {
}

void UAliveStatsComponent::CustomTick() {
}

void UAliveStatsComponent::ChangeHealth(float Value) {
}

void UAliveStatsComponent::CalculateStamina(bool bUse) {
}

float UAliveStatsComponent::ApplyDamage(AActor* causer, AActor* Weapon, float InputDamage) {
    return 0.0f;
}


