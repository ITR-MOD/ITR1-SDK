#include "PRGameInstance.h"

UPRGameInstance::UPRGameInstance() {
    this->StartupType = EStartupType::BuildShipping;
}

bool UPRGameInstance::SetShutdown() {
    return false;
}

void UPRGameInstance::RestartGame(const int32 Slot, const bool skipTutorial, const bool Ironman, const FRadiusGameDifficulty Difficulty) {
}




bool UPRGameInstance::IsEAPlayer() {
    return false;
}

bool UPRGameInstance::IsBetaPlayer() const {
    return false;
}

FString UPRGameInstance::HmdToString(EPRHmdDeviceType HMD) {
    return TEXT("");
}

bool UPRGameInstance::HideVersionRevision() const {
    return false;
}

bool UPRGameInstance::HasSupporterPack() const {
    return false;
}

void UPRGameInstance::HasSaveOnStartup(bool& hasSave) {
}

bool UPRGameInstance::GetPurchaseDate(FDateTime& PurchaseDate, FString& PurchaseDateString) {
    return false;
}

void UPRGameInstance::GetPlayerID(FString& ID) {
}

void UPRGameInstance::GetPlayerCountry(FString& countryCode) {
}

void UPRGameInstance::GetHMDDeviceType(EPRHmdDeviceType& HMD) {
}

void UPRGameInstance::GetDeviceTypeString(FString& HMD, FString& Controller) {
}

void UPRGameInstance::GetControllerType(EPRControllerType& Controller, bool& bIsDefault) {
}


void UPRGameInstance::FixTutorialSaveLevel() {
}


void UPRGameInstance::DoStart() {
}

FString UPRGameInstance::ControllerToString(EPRControllerType Controller) {
    return TEXT("");
}

//void UPRGameInstance::ConfigureSentrySubsystem_Delegate(USentrySettings* Settings) const {
//}


