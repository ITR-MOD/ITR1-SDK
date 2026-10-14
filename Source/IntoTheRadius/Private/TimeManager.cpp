#include "TimeManager.h"

UTimeManager::UTimeManager() {
}

bool UTimeManager::SetDateTime(const FDateTime NewDateTime) {
    return false;
}

bool UTimeManager::IsNight() const {
    return false;
}

bool UTimeManager::IsDay() const {
    return false;
}

UTimeManager* UTimeManager::GetTimeManager() {
    return NULL;
}

void UTimeManager::GetTime(int32& Hour, int32& Minute) {
}

float UTimeManager::GetSecondsTill(const float Hour) {
    return 0.0f;
}

float UTimeManager::GetSecondsPassed() {
    return 0.0f;
}

float UTimeManager::GetHoursPassed() {
    return 0.0f;
}

float UTimeManager::GetHour() {
    return 0.0f;
}

void UTimeManager::GetDateTimeStrings(FString& Date, FString& DayOfWeek, FString& Hour, FString& Minute, FString& Second) {
}

FDateTime UTimeManager::GetDateTime() {
    return FDateTime{};
}

void UTimeManager::DebugSetHour(const float Hour) {
}

void UTimeManager::AddTimeTill(float Hour) {
}

void UTimeManager::AddSeconds(const float Seconds) {
}

void UTimeManager::AddHours(const float Hours) {
}


