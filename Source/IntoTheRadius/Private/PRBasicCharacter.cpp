#include "PRBasicCharacter.h"

APRBasicCharacter::APRBasicCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->UseRelevanceManagement = false;
}

void APRBasicCharacter::SetCoverReservationFromSearchResult(const FNpcPointQueryResult& Result) {
}

void APRBasicCharacter::OnTransitionStart_Implementation() {
}

void APRBasicCharacter::OnTransitionEnd_Implementation() {
}

void APRBasicCharacter::OnLevelStarted_Implementation(FGameplayTag LevelTag) {
}

void APRBasicCharacter::OnLevelPrepare_Implementation(FGameplayTag LevelTag) {
}

void APRBasicCharacter::OnGameStartedPrivate() {
}

void APRBasicCharacter::OnGameStarted_Implementation() {
}

void APRBasicCharacter::OnGamePaused_Implementation(const bool bPaused) {
}

void APRBasicCharacter::OnGameDataSaved_Implementation() {
}

bool APRBasicCharacter::GetRandomReachablePoint(FVector Origin, float Radius, FVector StartPoint, float MinDistance, FVector& Point) {
    return false;
}

void APRBasicCharacter::ClearCoverReservation() {
}


