#include "PRNpcPerceptionComponent.h"
#include "PRPointOfInterest.h"

UPRNpcPerceptionComponent::UPRNpcPerceptionComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->bDebugInfoEnabled = false;
    this->PoiActorClass = APRPointOfInterest::StaticClass();
    this->AIEdPerceptionComponentClass = NULL;
    this->SenseDurationSpottedThreshold = 1.50f;
    this->SenseDurationLostThreshold = -2.00f;
    this->bForgetDead = true;
    this->bPoiFollowSelection = true;
    this->DebugHeight = 160.00f;
    this->AIPerceptionComponent = NULL;
    this->AIC = NULL;
    this->NpcPoiActor = NULL;
    this->PlayerRef = NULL;
    this->CurrentPlayerSelection = NULL;
    this->SenseTickTime = 0.10f;
    this->SightSenseRadius = 10.00f;
    this->SightSenseNightModifier = 1.00f;
    this->SightSenseAngleLeft = 45.00f;
    this->SightSenseAngleRight = 45.00f;
    this->SightSenseAngleUp = 30.00f;
    this->SightSenseAngleDown = 15.00f;
    this->HearingDistance = 15.00f;
    this->AwarenessLowValueLevel = 15.00f;
    this->AwarenessHighValueLevel = 15.00f;
    this->AwarenessMaxValue = 15.00f;
    this->AwarenessDecreaseMultiplier = 15.00f;
    this->AwarenessLevel = EAwarenessLevel::None;
    this->AwarenessValue = 0.00f;
}

void UPRNpcPerceptionComponent::VerifyPlayerSelection() {
}

void UPRNpcPerceptionComponent::UnselectPlayer() {
}

void UPRNpcPerceptionComponent::SpotPlayer() {
}

void UPRNpcPerceptionComponent::SpotActor(const AActor* Actor) {
}

void UPRNpcPerceptionComponent::SetupInitialState(float InitialAwareness, bool bPlayerInitiallySelected) {
}

void UPRNpcPerceptionComponent::SetSelection(const AActor* Actor, bool& bSuccess, AActor*& Selection) {
}

void UPRNpcPerceptionComponent::SetPoiFollowSelection(const bool bShouldFollow) {
}

void UPRNpcPerceptionComponent::SetPoi(const FVector& NewPoi, bool& bChanged, AActor*& POIActor) {
}

void UPRNpcPerceptionComponent::SelectPlayer() {
}

void UPRNpcPerceptionComponent::SelectionIsAlive(bool& bAlive) const {
}

void UPRNpcPerceptionComponent::IsSpotted(const AActor* Actor, bool& bSpotted) const {
}

void UPRNpcPerceptionComponent::IsPlayerSpotted(bool& bSpotted) const {
}

void UPRNpcPerceptionComponent::IsPlayerSelectionAlive(bool& bAlive) const {
}

void UPRNpcPerceptionComponent::IsEnemy(const AActor* Actor, bool& bIsEnemy) const {
}

void UPRNpcPerceptionComponent::IsActorSpottable(const AActor* Actor, bool& bSpottable) const {
}

void UPRNpcPerceptionComponent::HaveSelection(bool& bValid) {
}

void UPRNpcPerceptionComponent::GetSensedPlayerLocation(bool& bValid, FVector& Location) {
}

void UPRNpcPerceptionComponent::GetSelection(bool& bValid, AActor*& Selection, FTransform& BodyTransform) const {
}

void UPRNpcPerceptionComponent::GetPoi(bool& bValid, AActor*& POIActor) {
}

void UPRNpcPerceptionComponent::GetPlayerSelection(bool& bValid, AActor*& Player, FTransform& BodyTransform) const {
}

void UPRNpcPerceptionComponent::GetClosestActor(bool& bSuccess, AActor*& Actor) const {
}

void UPRNpcPerceptionComponent::GetAwareness(float& Value, EAwarenessLevel& Level) const {
}

void UPRNpcPerceptionComponent::ChangeAwarenessByTag(float Strength, FName Tag) {
}

void UPRNpcPerceptionComponent::ChangeAwareness(float Value) {
}

void UPRNpcPerceptionComponent::AIPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus) {
}


