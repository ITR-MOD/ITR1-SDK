#include "FLGeneral.h"
#include "Templates/SubclassOf.h"

UFLGeneral::UFLGeneral() {
}

FText UFLGeneral::TracersShowTypeToText(const ETracersShowType TracersShowType) {
    return FText::GetEmpty();
}

FString UFLGeneral::TracersShowTypeToString(const ETracersShowType TracersShowType) {
    return TEXT("");
}

FString UFLGeneral::TagsArrayAsString(const FString& Prefix, TArray<FGameplayTag> tagsArray) {
    return TEXT("");
}

AActor* UFLGeneral::SpawnActorWithLabel(const UObject* WorldContextObject, const TSubclassOf<AActor> ActorClass, const FVector& Location, const FString& Label) {
    return NULL;
}

void UFLGeneral::SortComponentsByLabel(TArray<USceneComponent*> In, TArray<USceneComponent*>& Out) {
}

void UFLGeneral::ShowMessage(const FString& Message, const FString& Title) {
}

FTransform UFLGeneral::ShiftActorByComponent(const AActor* Actor, const USceneComponent* Component, const FTransform& TargetTransform) {
    return FTransform{};
}

void UFLGeneral::SetLightVisibility(ULightComponent* Light, bool IsVisible) {
}

void UFLGeneral::SetLightAffectsWorld(ULightComponent* Component, bool bAffects) {
}

void UFLGeneral::SetCanEverAffectNavigation(UActorComponent* Component, bool bRelevant) {
}

void UFLGeneral::SetAngularRotationOffset(UPhysicsConstraintComponent* Component, const FRotator RotationOffset) {
}

void UFLGeneral::SetActorSimulatePhysics(const AActor* Actor, bool Simulate, bool zeroVel, bool& success, UPrimitiveComponent*& Primitive) {
}

bool UFLGeneral::SetActorEnableGravity(const AActor* Actor, bool enable) {
    return false;
}

void UFLGeneral::SafeDestroyActor(AActor* Actor) {
}

FGameplayTagContainer UFLGeneral::RequestGameplayTagChildren(const TArray<FGameplayTag>& GameplayTags) {
    return FGameplayTagContainer{};
}

void UFLGeneral::RefreshHolders(UObject* Object) {
}

void UFLGeneral::PutActorInPersistentLevel(AActor* Actor) {
}

float UFLGeneral::ProtectedApplyPointDamage(AActor* DamagedActor, float BaseDamage, const FVector& HitFromDirection, const FHitResult& HitInfo, AActor* ActorInstigator, AActor* DamageCauser, TSubclassOf<UDamageType> DamageTypeClass) {
    return 0.0f;
}

void UFLGeneral::PrintObjectReferences(UObject* Object) {
}

FRadiusGameDifficulty UFLGeneral::PresetToDifficulty(const EPRGameDifficultyPreset DifficultyPreset) {
    return FRadiusGameDifficulty{};
}

bool UFLGeneral::ParseCommandLine(const FString& ParameterName, FString& Value) {
    return false;
}

FMinMaxFloat UFLGeneral::Multiply_MinMaxFloat(FMinMaxFloat A, const float B) {
    return FMinMaxFloat{};
}

void UFLGeneral::KillActorWithChildren(AActor* Actor) {
}

void UFLGeneral::KillActorsByClass(const UObject* WorldContextObject, TSubclassOf<AActor> ActorsClass, bool bWithChildren) {
}

void UFLGeneral::KillActor(AActor* Actor) {
}

FText UFLGeneral::ItemsDropTypeToText(const EItemsDropType DropType) {
    return FText::GetEmpty();
}

FString UFLGeneral::ItemsDropTypeToString(const EItemsDropType DropType) {
    return TEXT("");
}

bool UFLGeneral::IsZeroParallelepiped(FParallelepiped Rect) {
    return false;
}

bool UFLGeneral::IsZeroInt(FMinMaxInt MinMax) {
    return false;
}

bool UFLGeneral::IsZeroFloat(FMinMaxFloat MinMax) {
    return false;
}

bool UFLGeneral::IsZeroDiffInt(FMinMaxInt MinMax) {
    return false;
}

bool UFLGeneral::IsZeroDiffFloat(FMinMaxFloat MinMax) {
    return false;
}

bool UFLGeneral::IsProfilingInCommandLine() {
    return false;
}

bool UFLGeneral::IsPreviewActor(const AActor* Actor) {
    return false;
}

bool UFLGeneral::IsPassedTutorialUnseen(FGameplayTag TriggerTag) {
    return false;
}

bool UFLGeneral::IsParallelepipedIntersectSphere(const FParallelepiped Rect, const FVector SphereCenter, const float SphereRadius) {
    return false;
}

bool UFLGeneral::IsParallelepipedIntersectPoint(const FParallelepiped Rect, const FVector Point) {
    return false;
}

bool UFLGeneral::IsNecessaryTutorial() {
    return false;
}

bool UFLGeneral::IsInTutorialLevel() {
    return false;
}

bool UFLGeneral::IsInMainMenuLevel() {
    return false;
}

bool UFLGeneral::IsInBoundsIgnoreZ(const FVector& Point, const FTransform& BoxWorldTransform, const FVector& BoxExtent) {
    return false;
}

bool UFLGeneral::IsGameStarted() {
    return false;
}

bool UFLGeneral::IsGameRunning(const UObject* WorldContextObject) {
    return false;
}

bool UFLGeneral::IsGameplayRunning() {
    return false;
}

bool UFLGeneral::IsGameClosing() {
    return false;
}

bool UFLGeneral::IsFreeGame() {
    return false;
}

bool UFLGeneral::IsEditorCustomStartup() {
    return false;
}

bool UFLGeneral::IsEarlyAccessPlayer() {
    return false;
}

bool UFLGeneral::IsDifficultyChanged(const FRadiusGameDifficulty NewDifficulty) {
    return false;
}

bool UFLGeneral::IsDifficultyAppliedAfterTide(const FRadiusGameDifficulty NewDifficulty) {
    return false;
}

bool UFLGeneral::IsDefaultUpgrade(const FGameplayTag UpgradeID, UInventoryItem* Item) {
    return false;
}

bool UFLGeneral::IsCorpsesCensored() {
    return false;
}

bool UFLGeneral::IsConditionsPassed(const TArray<UPRCondition*> Conditions) {
    return false;
}

bool UFLGeneral::IsComponentBoundsIntersect(const UPrimitiveComponent* ComponentA, const UPrimitiveComponent* ComponentB) {
    return false;
}

bool UFLGeneral::IsCommandLine(const FString& ParameterName) {
    return false;
}

bool UFLGeneral::IsActorInLevel(const AActor* Actor, const FGameplayTag LevelTag) {
    return false;
}

bool UFLGeneral::IsActorInCurrentLevel(const AActor* Actor) {
    return false;
}

bool UFLGeneral::HasUnseenPassedTutorials() {
    return false;
}

bool UFLGeneral::HasPassedTutorials() {
    return false;
}

float UFLGeneral::GetTrueMin(FMinMaxFloat MinMax) {
    return 0.0f;
}

float UFLGeneral::GetTrueMax(FMinMaxFloat MinMax) {
    return 0.0f;
}

void UFLGeneral::GetTimeStringFromSeconds(int32 TotalSeconds, bool bNoSeconds, FString& String) {
}

void UFLGeneral::GetTimeSpentString(int32 TotalMinutes, FString& String) {
}

FName UFLGeneral::GetStreamingLevelNameFromActor(const AActor* Actor) {
    return NAME_None;
}

float UFLGeneral::GetSignedAngle(FVector Normal, FVector v1, FVector v2) {
    return 0.0f;
}

FTransform UFLGeneral::GetRelativeTransform(const AActor* Actor) {
    return FTransform{};
}

int32 UFLGeneral::GetRandomInt(FMinMaxInt MinMax) {
    return 0;
}

float UFLGeneral::GetRandomFloat(FMinMaxFloat MinMax) {
    return 0.0f;
}

void UFLGeneral::GetPRPlayerPawn(const UObject* WorldContextObject, APawn*& prPlayerPawn) {
}

FVector UFLGeneral::GetPointsBoxSize(const TArray<FVector> Points) {
    return FVector{};
}

UClass* UFLGeneral::GetParentClass(UClass* Class) {
    return NULL;
}

UClass* UFLGeneral::GetObjectParentClass(UObject* Object) {
    return NULL;
}

FName UFLGeneral::GetObjectFName(const UObject* Object) {
    return NAME_None;
}

FString UFLGeneral::GetNewNoteMarker(const FGameplayTag& Tag) {
    return TEXT("");
}

FTutorialConfig UFLGeneral::GetNecessaryTutorialConfig() {
    return FTutorialConfig{};
}

void UFLGeneral::GetMapLevelTag(const UObject* WorldContextObject, FGameplayTag& LevelTag) {
}

void UFLGeneral::GetLevelByName(const FString& LevelName, FGameplayTag& LevelTag) {
}

FBox UFLGeneral::GetLevelBounds(ULevel* TargetLevel, FVector& LevelLocation) {
    return FBox{};
}

bool UFLGeneral::GetGamePurchaseDate(FDateTime& PurchaseDate, FString& PurchaseDateString) {
    return false;
}

void UFLGeneral::GetDateTimeString(FDateTime DateTime, FString& String) {
}

UObject* UFLGeneral::GetAttackFromPawn(APawn* Pawn) {
    return NULL;
}

TArray<FGameplayTag> UFLGeneral::GetAllSecondaryWeaponTags() {
    return TArray<FGameplayTag>();
}

TArray<FGameplayTag> UFLGeneral::GetAllPrimaryWeaponTags() {
    return TArray<FGameplayTag>();
}

TArray<FGameplayTag> UFLGeneral::GetAllCassetteTags() {
    return TArray<FGameplayTag>();
}

TArray<FGameplayTag> UFLGeneral::GetAllArtifactTags() {
    return TArray<FGameplayTag>();
}

FGameplayTag UFLGeneral::GetActorLevelTag(const AActor* Actor) {
    return FGameplayTag{};
}

FString UFLGeneral::GetActorLevelName(const AActor* Actor) {
    return TEXT("");
}

void UFLGeneral::GetActorHolderTransform(const AActor* Actor, const FGameplayTag HolderID, bool& success, FTransform& RelativeTransform) {
}

FString UFLGeneral::GameDefaultMap() {
    return TEXT("");
}

int32 UFLGeneral::FindIndexOfTag(const FGameplayTag& Tag, const TArray<FGameplayTag>& Array, bool bExact) {
    return 0;
}

float UFLGeneral::FindDistanceAlongSplineClosestToLocation(const USplineComponent* Spline, const FVector& Location) {
    return 0.0f;
}

void UFLGeneral::DrawArrow(const UObject* WorldContextObject, const FVector LineStart, const FVector LineEnd, const float ArrowSize, const FLinearColor Color, const float Duration, const float Thickness) {
}

FText UFLGeneral::DifficultyTypeToText(const EDifficultyType DifficultyType) {
    return FText::GetEmpty();
}

FString UFLGeneral::DifficultyTypeToString(const EDifficultyType DifficultyType) {
    return TEXT("");
}

EPRGameDifficultyPreset UFLGeneral::DifficultyToPreset(const FRadiusGameDifficulty GameDifficulty) {
    return EPRGameDifficultyPreset::Easy;
}

FText UFLGeneral::DifficultyPresetToText(const EPRGameDifficultyPreset DifficultyPreset) {
    return FText::GetEmpty();
}

FString UFLGeneral::DifficultyPresetToString(const EPRGameDifficultyPreset DifficultyPreset) {
    return TEXT("");
}

void UFLGeneral::DifficultiesToNames(TArray<EDifficultyType> Difficulties, TArray<FText>& Names, TArray<int32>& Indexes) {
}

void UFLGeneral::ClearUnseenPassedTutorials() {
}

void UFLGeneral::CheckUniqueInstanceID(const UObject* WorldContextObject, FString& UID, const FString& Prefix, const bool bOnlyIfEmpty) {
}

FVector UFLGeneral::CalcLocalBounds(USceneComponent* Component) {
    return FVector{};
}

bool UFLGeneral::BlinkFloorSweepTest(UCharacterMovementComponent* MovementComponent, ACharacter* CharacterOwner, FHitResult& Hit, const FVector& CapsuleLocation, const FVector& TraceStart, const FVector& TraceEnd) {
    return false;
}

TArray<FGameplayTag> UFLGeneral::AsArray(const FGameplayTagContainer& Container) {
    return TArray<FGameplayTag>();
}

FPRNPCInfo UFLGeneral::ApplyDifficultyModifiers(FPRNPCInfo NpcInfo) {
    return FPRNPCInfo{};
}

FString UFLGeneral::AmmoContainerDataAsString(const FString& Prefix, FAmmoContainerData AmmoContainerData) {
    return TEXT("");
}

UActorComponent* UFLGeneral::AddComponentToActor(AActor* BaseActor, TSubclassOf<UActorComponent> AttachedComponent) {
    return NULL;
}


