#include "DataSaveManager.h"

UDataSaveManager::UDataSaveManager() {
}

void UDataSaveManager::SetUseAlternateSaveLocation(bool Value) {
}

bool UDataSaveManager::SaveStringToFileEditor(const FString& String, const FString& FilePath, const bool bAppend) {
    return false;
}

bool UDataSaveManager::SaveStringToFile(const FString& String, const FString& FilePath, const bool bAppend) {
    return false;
}

void UDataSaveManager::SaveGameData(UPRGameData* Data, int32 SlotNr, int32 SaveNr) {
}

bool UDataSaveManager::IsSlotWithIronman(const int32 Slot) {
    return false;
}

bool UDataSaveManager::IsSlotEmpty(const FSaveSlotInfo& SlotInfo) {
    return false;
}

bool UDataSaveManager::HasSaves() {
    return false;
}

bool UDataSaveManager::GetUseAlternateSaveLocation() const {
    return false;
}

bool UDataSaveManager::GetSlotInfo(const int32 Slot, FSaveSlotInfo& SlotInfo) {
    return false;
}

FString UDataSaveManager::GetSavesPath() {
    return TEXT("");
}

bool UDataSaveManager::GetSaveInfo(const int32 Slot, const int32 Save, FSaveInfo& SaveInfo) {
    return false;
}

FSaveInfo UDataSaveManager::GetLastSaveInfo(FSaveSlotInfo SlotInfo) {
    return FSaveInfo{};
}

UDataSaveManager* UDataSaveManager::GetDataSaveManager() {
    return NULL;
}

FString UDataSaveManager::GetDataPath() {
    return TEXT("");
}

bool UDataSaveManager::FixInventoryItemPosition(UInventoryItem* Item) {
    return false;
}

void UDataSaveManager::CopyScopedToAlternateSave() {
}

void UDataSaveManager::CopyAlternateToScopedSave() {
}

void UDataSaveManager::AutosaveGameData(const FString& Message) {
}


