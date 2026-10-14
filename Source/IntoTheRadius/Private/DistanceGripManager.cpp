#include "DistanceGripManager.h"

UDistanceGripManager::UDistanceGripManager() {
    this->GripSystemOwner = NULL;
}

void UDistanceGripManager::LevelTransitionStartCallback() {
}

void UDistanceGripManager::LevelTransitionEndCallback() {
}

void UDistanceGripManager::ItemTakenFromLevelCallback(UInventoryItem* ItemTaken) {
}

void UDistanceGripManager::ItemStoredToLevelCallback(UInventoryItem* ItemStored) {
}

void UDistanceGripManager::ItemRemovedFromLevelCallback(UInventoryItem* ItemRemoved) {
}

void UDistanceGripManager::ItemActorRemovedCallback(UInventoryItem* ItemWithActorRemoved) {
}


