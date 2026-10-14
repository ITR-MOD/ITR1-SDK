#include "DiscordObject.h"

class UDiscordObject;

void UDiscordObject::StopDiscordTimer() {
}

void UDiscordObject::StartDiscordTimer() {
}

void UDiscordObject::SetState(const FString& InNewState) {
}

void UDiscordObject::SetSmallImageText(const FString& InKeyName) {
}

void UDiscordObject::SetSmallImage(const FString& InKeyName) {
}

void UDiscordObject::SetPartySize(const int32 InNewPartySize) {
}

void UDiscordObject::SetPartyMax(const int32 InNewPartyMax) {
}

void UDiscordObject::SetPartyId(const FString& InNewPartyId) {
}

void UDiscordObject::SetLargeImageText(const FString& InKeyName) {
}

void UDiscordObject::SetLargeImage(const FString& InKeyName) {
}

void UDiscordObject::SetJoinSecret(const FString& InNewJoinSecret) {
}

void UDiscordObject::SetDetails(const FString& InNewDetails) {
}

FString UDiscordObject::GetDiscordResultString(EDiscordReturnResult InDiscordResult) {
    return TEXT("");
}

UDiscordObject* UDiscordObject::GetDiscordObject() {
    return NULL;
}

void UDiscordObject::DestroyDiscordObject() {
}

void UDiscordObject::CreateDiscordObject(const FString& InClientID, const bool bRequireDiscordRunning, const bool bStartElapsedTimer) {
}

UDiscordObject::UDiscordObject() {
}

