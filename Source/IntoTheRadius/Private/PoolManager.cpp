#include "PoolManager.h"
#include "Templates/SubclassOf.h"

UPoolManager::UPoolManager() {
}

bool UPoolManager::ReturnActor(AActor* Actor) {
    return false;
}

void UPoolManager::PrefillPoolFromConfig() {
}

bool UPoolManager::IsActorInPool(const AActor* Actor) {
    return false;
}

UPoolManager* UPoolManager::GetPoolManager() {
    return NULL;
}

AActor* UPoolManager::GetActor(const TSubclassOf<AActor> ActorClass, const FTransform Transform, AActor* Owner, APawn* Instigator) {
    return NULL;
}


