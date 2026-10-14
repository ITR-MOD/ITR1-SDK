#include "MonsterSpawner.h"
#include "Components/SphereComponent.h"

AMonsterSpawner::AMonsterSpawner(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->RootComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SpawnSphere"));
    this->Sphere = (USphereComponent*)RootComponent;
    this->Wild = true;
    this->bForceInstanceID = false;
    this->Radius = 1000.00f;
    this->CanWalkAway = false;
    this->SnipingMode = false;
    this->AlternativeMonstersMax = 0;
}

void AMonsterSpawner::ToggleNpcSummary(UObject* WorldContextObj) {
}

void AMonsterSpawner::SpawnSavedNPC(bool& bSuccess, AActor*& NPCActor) {
}

void AMonsterSpawner::SpawnNPC(bool& bSuccess, AActor*& NPCActor) {
}

void AMonsterSpawner::OnNPCDied(AActor* NPCActor) {
}

TMap<FGameplayTag, int32> AMonsterSpawner::NPCsListToMonstersMap(const TArray<FGameplayTag>& List) {
    return TMap<FGameplayTag, int32>();
}

bool AMonsterSpawner::IsNpcSummaryEnabled() {
    return false;
}

bool AMonsterSpawner::IsFull() {
    return false;
}


