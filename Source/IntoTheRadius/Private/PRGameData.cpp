#include "PRGameData.h"

UPRGameData::UPRGameData() {
    this->BackupPlayerData = NULL;
    this->PlayerData = NULL;
    this->MissionsData = NULL;
    this->InventoryData = NULL;
    this->SpawnSaveData = NULL;
    this->UID = 10;
    this->SlotNr = 0;
    this->TicksPassed = 0;
    this->LastTideTicks = -1;
    this->Difficulty = EPRGameDifficultyPreset::Normal;
    this->Ironman = false;
    this->IntactDifficulty = true;
    this->GameUID = TEXT("-none-");
    this->GameStartDateString = TEXT("-unknown-");
    this->GameRealTicks = 0;
}

void UPRGameData::UpdateMarker(const FString& Marker, const bool bRemove) {
}

bool UPRGameData::UnsetTrigger(const FGameplayTag& Trigger) {
    return false;
}

bool UPRGameData::StatAddUniqueItem(const FGameplayTag ItemId) {
    return false;
}

void UPRGameData::StatAddKill(const FGameplayTag NPCTag, const FGameplayTag weaponTag) {
}

void UPRGameData::StatAddDeath(const FGameplayTag KillerTag) {
}

bool UPRGameData::SetTrigger(const FGameplayTag& Trigger) {
    return false;
}

void UPRGameData::SetPlayerLevel(const int32 PlayerLevel) {
}

void UPRGameData::SaveSleepLocation(const FGameplayTag Level, const FVector Location) {
}

void UPRGameData::SaveDecorItemTransform(const FString& DecorId, const FTransform Transform) {
}

void UPRGameData::SaveDecorItemDestroyed(const FString& DecorId) {
}

void UPRGameData::SaveDeathLocation(const FGameplayTag Level, const FVector Location) {
}

void UPRGameData::ResetGameData() {
}

void UPRGameData::RemoveSleepLocation(const FGameplayTag Level) {
}

void UPRGameData::RemoveDeathLocation(const FGameplayTag Level) {
}

void UPRGameData::PrintMissions(const FString& Message) {
}

void UPRGameData::PrintInventory(const FString& Message) {
}

void UPRGameData::PrintData(const FString& Message) {
}

void UPRGameData::PrepareNewGame(const FRadiusGameDifficulty NewDifficulty, const bool bIronman, const bool bSkipTutorial, const bool bStartupConfig, const TArray<FGameplayTag> Triggers) {
}

void UPRGameData::OnPlayerLevelChanged(int32 PlayerLevel) {
}

void UPRGameData::OnObjectiveChange(UMissionData* MissionData, UMissionAbstractObjectiveData* Objective) {
}

void UPRGameData::OnMissionChange(UMissionData* MissionData) {
}

void UPRGameData::OnItemTaken(UInventoryItem* Item) {
}

void UPRGameData::OnItemStored(UInventoryItem* Item) {
}

bool UPRGameData::IsManOfHisWord() const {
    return false;
}

bool UPRGameData::IsKatyaMonologueInvalid(const FGameplayTag& Tag, const bool bIgnoreConditions) {
    return false;
}

bool UPRGameData::IsJournalGroupHasNewNotes(EJournalGroup Group) const {
    return false;
}

bool UPRGameData::HasMoney(const int32 Amount) const {
    return false;
}

FString UPRGameData::GetUID() {
    return TEXT("");
}

void UPRGameData::GetSpawnSaveData(USpawnerGameData*& NewSpawnSaveData) {
}

bool UPRGameData::GetSleepLocation(const FGameplayTag Level, FVector& Location) const {
    return false;
}

float UPRGameData::GetSecondsPassed() const {
    return 0.0f;
}

float UPRGameData::GetSecondsLastTide() const {
    return 0.0f;
}

float UPRGameData::GetRealSecondsToTide() {
    return 0.0f;
}

float UPRGameData::GetPlaytimeSeconds() const {
    return 0.0f;
}

float UPRGameData::GetPlaytimeMinutes() const {
    return 0.0f;
}

int32 UPRGameData::GetPlayerLevel() const {
    return 0;
}

void UPRGameData::GetMapRenderTarget(const FGameplayTag LevelTag, UTextureRenderTarget2D*& RenderTarget) {
}

TArray<FNoteData> UPRGameData::GetJournalGroupNotes(EJournalGroup Group) {
    return TArray<FNoteData>();
}

float UPRGameData::GetGameSecondsToTide() {
    return 0.0f;
}

void UPRGameData::GetGameData(UPRGameData*& GameData) {
}

EPRGameDifficultyPreset UPRGameData::GetDifficultyPreset() const {
    return EPRGameDifficultyPreset::Easy;
}

void UPRGameData::GetDecorItemTransform(const FString& DecorId, FTransform& Transform, bool& bDestroyed, bool& bSuccess) {
}

bool UPRGameData::GetDeathLocation(const FGameplayTag Level, FVector& Location) const {
    return false;
}

void UPRGameData::FillFrom(UPRGameData* Other) {
}

void UPRGameData::DumpData(const FString& Message) {
}

bool UPRGameData::CheckTrigger(const FGameplayTag& Trigger) {
    return false;
}

void UPRGameData::CheckTideTriggers() {
}

bool UPRGameData::CheckMarker(const FString& Marker) {
    return false;
}

void UPRGameData::CheckGameTriggers() {
}

void UPRGameData::ChangeMoney(const int32 ChangeBy, const TArray<FString>& AnalyticsParams) {
}

bool UPRGameData::ChangeGameDifficulty(const FRadiusGameDifficulty Value) {
    return false;
}

void UPRGameData::ApplyTideReset() {
}


