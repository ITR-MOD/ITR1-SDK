#include "AmmoConfig.h"

UAmmoConfig::UAmmoConfig() {
    this->CommonInfoTable = NULL;
    this->MeshPrimitiveDataIndex = 6;
    this->ProjectileForwardSpawnOffset = 10.00f;
    this->BulletClassActor = NULL;
    this->GrenadeClassActor = NULL;
    this->ShrapnelClassActor = NULL;
    this->ExplosionClassActor = NULL;
}

void UAmmoConfig::SetupAmmoMeshComponent(const FGameplayTag& AmmoTag, UStaticMeshComponent* AmmoMeshComponent, const bool IsFired) {
}

bool UAmmoConfig::IsTakenOutAmmo(AActor* Actor) {
    return false;
}

bool UAmmoConfig::IsShrapnelAmmo(FGameplayTag AmmoTag) {
    return false;
}

bool UAmmoConfig::IsHeldAmmo(AActor* Actor) {
    return false;
}

bool UAmmoConfig::IsAmmo(const AActor* Actor) {
    return false;
}

UStaticMesh* UAmmoConfig::GetAmmoWholeMesh(const FGameplayTag& AmmoType) {
    return NULL;
}

FText UAmmoConfig::GetAmmoTypeName(const FGameplayTag& AmmoType) {
    return FText::GetEmpty();
}

FLinearColor UAmmoConfig::GetAmmoTypeColor(const FGameplayTag& AmmoType) {
    return FLinearColor{};
}

UAmmoConfig* UAmmoConfig::GetAmmoTagsConfig() {
    return NULL;
}

UStaticMesh* UAmmoConfig::GetAmmoShellMesh(const FGameplayTag& AmmoType) {
    return NULL;
}


