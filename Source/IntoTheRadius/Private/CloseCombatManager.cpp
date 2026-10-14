#include "CloseCombatManager.h"

UCloseCombatManager::UCloseCombatManager() {
    this->PlayerRef = NULL;
}

void UCloseCombatManager::UnregisterNpc(ACharacter* NpcToUnregister) {
}

void UCloseCombatManager::ToggleDebug() {
}

void UCloseCombatManager::RegisterNpc(ACharacter* NpcToRegister) {
}

void UCloseCombatManager::InitData() {
}

float UCloseCombatManager::GetTeleportSearchRadius() const {
    return 0.0f;
}

float UCloseCombatManager::GetMinDistanceFromPlayerToReappear() const {
    return 0.0f;
}

bool UCloseCombatManager::GetDebugEnabled() const {
    return false;
}

ENpcCombatState UCloseCombatManager::GetCloseCombatState(ACharacter* NPC) const {
    return ENpcCombatState::Uninitialized;
}

UCloseCombatManager* UCloseCombatManager::GetCloseCombatManager() {
    return NULL;
}

void UCloseCombatManager::DebugSetBudget(const float NewBudget) {
}

void UCloseCombatManager::DebugGetBudget(float& CCBudget, float& Weight) {
}

void UCloseCombatManager::CleanData() {
}


