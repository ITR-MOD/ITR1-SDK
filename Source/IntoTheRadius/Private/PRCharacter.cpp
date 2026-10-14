#include "PRCharacter.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Components/AudioComponent.h"
#include "Components/WidgetComponent.h"
#include "AIEdPerceptionComp.h"
#include "AliveStatsComponent.h"
#include "PRNpcPerceptionComponent.h"

APRCharacter::APRCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->DebugWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("DebugWidget"));
    this->PerceptionComponent = CreateDefaultSubobject<UPRNpcPerceptionComponent>(TEXT("Npc Perception Component"));
    this->AudioIdle = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio Idle Component"));
    this->AIPerceptionStimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Perception Stimuli Source Component"));
    this->AIEdPerceptionComponent = CreateDefaultSubobject<UAIEdPerceptionComp>(TEXT("AI Ed Perception Component"));
    this->AliveStatsComponent = CreateDefaultSubobject<UAliveStatsComponent>(TEXT("Alive Stats Component"));
    this->BTAsset = NULL;
    this->WalkSpeed = 1.00f;
    this->IdleTime = 2.00f;
    this->bLookAroundAfterWalk = false;
    this->LootPossibility = 1.00f;
    this->IdleOrigin = NULL;
    this->bStrictOrigin = false;
    this->bDebugInfo = false;
    this->CritComponent = NULL;
    this->IdleWalkRadius = 1000.00f;
    this->CharName = TEXT("Monster");
    this->LootPossibilityOverride = -1.00f;
    this->Command = ECharCommand::Idle;
    this->ValidDistanceToPlayer = 250.00f;
    this->bEnabled = false;
    this->bAlertCooldown = false;
    this->CurrentDetailLevel = ECharDetailLevel::Normal;
    this->AIC = NULL;
    this->BB = NULL;
    this->bLerpMovement = false;
    this->LootItemClass = NULL;
    this->ShootingComponent = NULL;
    this->DebugWidget->SetupAttachment(RootComponent);
    this->PerceptionComponent->SetupAttachment(RootComponent);
    this->AudioIdle->SetupAttachment(RootComponent);
}

void APRCharacter::WalkToLocation(const FVector Location) {
}

void APRCharacter::WalkToActorLocation(AActor* Actor) {
}

void APRCharacter::SwitchAiDebug() {
}

void APRCharacter::SpawnLoot() const {
}

void APRCharacter::SetupState(const FNPCGameData& NPC_data, TArray<FName> BoolNames, TArray<bool> BoolValues) {
}

void APRCharacter::SetEnableEverything(const bool enable) {
}

void APRCharacter::SetEnableAdditionalThings_Implementation(bool enable) {
}

void APRCharacter::SetCommand(ECharCommand NewCommand, bool Force) {
}

void APRCharacter::SetBrainEnabled(const bool enable) const {
}

bool APRCharacter::SelectAndAttackClosest() {
    return false;
}

bool APRCharacter::SelectAndAttack(AActor* Actor) {
    return false;
}

void APRCharacter::SaveGameData() {
}

void APRCharacter::RestartTree() const {
}

void APRCharacter::ReappearOutOfSight() {
}

void APRCharacter::ReappearCoverCallback(const FNpcPointQueryResult& QueryResult) {
}

void APRCharacter::ProcessNoise(const FName Tag, const FVector Location) {
}

void APRCharacter::ProcessDamage(AActor* causer, float Damage) {
}

void APRCharacter::PrintMessage(const FString& MessageToPrint) const {
}

void APRCharacter::OnHeardNoise(AActor* Actor, FVector Location, float Strength, FName Tag) {
}

void APRCharacter::OnDeath_Implementation(bool bEnableRagdoll) {
}

void APRCharacter::OnDamaged_Implementation(AActor* causer, const float Damage) {
}

void APRCharacter::NpcAwarenessLevelChanged(const EAwarenessLevel PrevLevel, const EAwarenessLevel NewLevel) {
}

void APRCharacter::LoadBaseSettings() {
}

void APRCharacter::LoadAdditionalSettings_Implementation() {
}

bool APRCharacter::IsPlayerNear() const {
    return false;
}

bool APRCharacter::IsLocInValidRange(FVector Location) const {
    return false;
}

bool APRCharacter::IsEnabled() const {
    return false;
}

void APRCharacter::InvestigateLocation(FVector Location) {
}

void APRCharacter::HealthEnd() {
}

FVector APRCharacter::GetIdleOriginPoint() const {
    return FVector{};
}

float APRCharacter::GetFootstepNoiseRange() const {
    return 0.0f;
}

EAwarenessLevel APRCharacter::GetAwarenessLevel() const {
    return EAwarenessLevel::None;
}

float APRCharacter::GetAttackMoveDilationAngle() const {
    return 0.0f;
}

float APRCharacter::GetAttackLostLocationTime() const {
    return 0.0f;
}

void APRCharacter::EnableRagdoll() {
}

void APRCharacter::DestroyController() {
}

void APRCharacter::CharEnabled_Implementation() {
}

void APRCharacter::AlertNearbyMonsters(AActor* causer, const bool bDeath) {
}


