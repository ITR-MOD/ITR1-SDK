#include "PRAnomaly.h"
#include "Components/AudioComponent.h"
#include "Components/SceneComponent.h"

APRAnomaly::APRAnomaly(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    this->RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    this->AnomalyRadius = 300.00f;
    this->Root = (USceneComponent*)RootComponent;
    this->Audio = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio"));
    this->Trigger = NULL;
    this->MinVisTrigger = 40;
    this->MaxVisTrigger = 120;
    this->SignificanceDistance = 10000.00f;
    this->EffectDelay = 0.20f;
    this->AlwaysSignificant = false;
    this->Significant = true;
    this->IsAnomalyTriggered = false;
    this->RandomTime = 0;
    this->AnomalyBaseDamage = 0.00f;
    this->ConstSound = NULL;
    this->DamageType = NULL;
    this->SelfTriggering = false;
    this->NDebug = false;
    this->TriggeredOverlapCheckTimer = 0.30f;
    this->IsSoundConstant = true;
    this->DotDuration = 3.00f;
    this->HasAudio = false;
    this->AffectsOnlyPlayer = false;
    this->BeginOverlapAffectsActorsMultipleTimesIfBlink = false;
    this->IgnorePlayerInventory = false;
    this->Audio->SetupAttachment(RootComponent);
}

void APRAnomaly::TriggerVisuals() {
}




void APRAnomaly::OverlapCheckerDelegate() {
}


void APRAnomaly::OnOverlapEndTrigger(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
}

void APRAnomaly::OnOverlapBeginTrigger(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
}

void APRAnomaly::LevelStarted(FGameplayTag LevelTag) {
}

void APRAnomaly::LevelPrepare(FGameplayTag LevelTag) {
}

void APRAnomaly::GenerateTriggersOverlapEvents(bool ClearExistingEvents) {
}

void APRAnomaly::ApplyAnomalyDamage(AActor* Actor) {
}



