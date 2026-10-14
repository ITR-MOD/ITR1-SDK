#include "INIManager.h"

UINIManager::UINIManager() {
}

bool UINIManager::TrySetVector(const FString& Filename, const FString& Section, const FString& Key, const FVector Value) {
    return false;
}

bool UINIManager::TrySetString(const FString& Filename, const FString& Section, const FString& Key, const FString& Value) {
    return false;
}

bool UINIManager::TrySetInt(const FString& Filename, const FString& Section, const FString& Key, const int32 Value) {
    return false;
}

bool UINIManager::TrySetFloat(const FString& Filename, const FString& Section, const FString& Key, const float Value) {
    return false;
}

bool UINIManager::TrySetBool(const FString& Filename, const FString& Section, const FString& Key, const bool bValue) {
    return false;
}

FVector UINIManager::TryGetVector(const FString& Filename, const FString& Section, const FString& Key) {
    return FVector{};
}

FString UINIManager::TryGetString(const FString& Filename, const FString& Section, const FString& Key) {
    return TEXT("");
}

int32 UINIManager::TryGetInt(const FString& Filename, const FString& Section, const FString& Key) {
    return 0;
}

float UINIManager::TryGetFloat(const FString& Filename, const FString& Section, const FString& Key) {
    return 0.0f;
}

bool UINIManager::TryGetBool(const FString& Filename, const FString& Section, const FString& Key) {
    return false;
}

void UINIManager::StoreInUserSave(const FString& Filename) {
}

FString UINIManager::GetStringSilent(const FString& Filename, const FString& Section, const FString& Key) {
    return TEXT("");
}

UINIManager* UINIManager::GetINIManager() {
    return NULL;
}

float UINIManager::GetFloatSilent(const FString& Filename, const FString& Section, const FString& Key) {
    return 0.0f;
}

bool UINIManager::GetBoolSilent(const FString& Filename, const FString& Section, const FString& Key) {
    return false;
}

bool UINIManager::Exists(const FString& Filename, const FString& Section, const FString& Key) {
    return false;
}


