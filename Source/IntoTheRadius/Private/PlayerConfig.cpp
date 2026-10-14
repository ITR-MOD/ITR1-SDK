#include "PlayerConfig.h"

UPlayerConfig::UPlayerConfig() {
    this->StartMaxPlayerHP = 100.00f;
    this->HealthBoothPriceHealth = 2;
    this->HealthBoothPriceRest = 3;
    this->SleepHealthIncreaseSpeed = 0.00f;
    this->StartWalkSpeed = 220.00f;
    this->StartSprintSpeed = 750.00f;
    this->StartCrouchSpeed = 90.00f;
    this->TeleportTraceDownDistance = 2000.00f;
    this->TeleportSweepSteps = false;
    this->TeleportSpeed = 200.00f;
    this->TeleportMaxStepHeight = 50.00f;
    this->TeleportAnomaliesTimerDivisor = 5.00f;
    this->PoisonedWaterDot = 10.00f;
    this->WaterDrowningDot = 25.00f;
    this->WaterStaminaDot = 15.00f;
    this->MaxPlayerStamina = 100.00f;
    this->StartSprintStaminaUsage = 0.22f;
    this->StaminaUsageCoefficient = 0.01f;
    this->StaminaRegenerationSpeed = 0.07f;
    this->MaxPlayerRest = 100.00f;
    this->StartRestIncreaseSpeed = 0.00f;
    this->StartRestDecreaseSpeed = 0.00f;
    this->MaxPlayerNutrition = 100.00f;
    this->MaxPlayerHunger = 100.00f;
    this->HungerSpeed = 0.00f;
    this->HungerTolerance = 40.00f;
    this->ShortForwardGripRadius = 15.00f;
    this->DistanceGripDeadZone = 35.00f;
    this->DistanceGripSphereTraceRadius = 60.00f;
    this->DistanceGripTraceLength = 200.00f;
    this->LoadAmmoContainersIfInHands = true;
    this->LoadAmmoContainersIfInInventory = false;
    this->LoadAmmoBoxesIfNotInHands = true;
    this->DeathItemsDropRadius = 300.00f;
    this->InventoryItemsPositionTreshold = 1500.00f;
    this->SystemSubtitlesType = EPRSubtitleType::Unknown;
    this->CameraSubtitlesType = EPRSubtitleType::Player;
    this->InspectShowDelay = 2.00f;
    this->InspectHideDelay = 0.50f;
    this->InspectShowDistance = 40.00f;
}

void UPlayerConfig::GetWeightToWarningColor(const float Weight, FLinearColor& Color) {
}

void UPlayerConfig::GetWeightToWalkSpeedCoefficient(const float Weight, float& SpeedCoefficient) {
}

void UPlayerConfig::GetWeightToStaminaCoefficient(const float Weight, float& Coefficient) {
}

void UPlayerConfig::GetWeightToRunSpeedCoefficient(const float Weight, float& SpeedCoefficient) {
}

void UPlayerConfig::GetWaterMovementReductionCoefficient(float BodyInWaterRatio, float& MovementCoefficient) {
}

float UPlayerConfig::GetStaminaRegenerationSpeed() const {
    return 0.0f;
}

UPlayerConfig* UPlayerConfig::GetPlayerConfig() {
    return NULL;
}

void UPlayerConfig::GetFallDamage(float SpeedDelta, float& Damage) {
}


