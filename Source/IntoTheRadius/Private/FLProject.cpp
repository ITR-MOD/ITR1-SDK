#include "FLProject.h"

UFLProject::UFLProject() {
}

FString UFLProject::TrimFloat(const float In, const int32 Digits) {
    return TEXT("");
}

FVector UFLProject::RoundVector(const FVector& In, const int32 Digits) {
    return FVector{};
}

FTransform UFLProject::RoundTransform(const FTransform& In) {
    return FTransform{};
}

FRotator UFLProject::RoundRotator(const FRotator& In, const int32 Digits) {
    return FRotator{};
}

float UFLProject::RoundFloat(const float In, const int32 Digits) {
    return 0.0f;
}

float UFLProject::Negative(const float& In) {
    return 0.0f;
}

TArray<UObject*> UFLProject::NativeSortObjectsByDistance(const TArray<UObject*> inArray, const AActor* DistanceFromActor) {
    return TArray<UObject*>();
}

bool UFLProject::IsInputEnabledAndNotSimulation() {
    return false;
}

bool UFLProject::GetSpawnPoint(const FVector& nearby, const float InRadius, const bool reachable, FVector& OutPoint) {
    return false;
}

bool UFLProject::GetReachablePointNotVisibleToPlayer(const APawn* PlayerPawn, const APawn* NPC, float MinDistFromPlayer, float SearchRadius, int32 NumAttempts, FVector& OutPoint) {
    return false;
}

bool UFLProject::GetNavigablePointBehindPlayer(const APawn* PlayerPawn, float MinDistFromPlayer, float SearchRadius, FVector& OutPoint) {
    return false;
}

bool UFLProject::GetGripRelativeTransform(UObject* GripObject, UGripMotionControllerComponent* HandController, EPRHandPoseType PoseType, FTransform& OutGripRelativeTransform) {
    return false;
}

void UFLProject::GetGripObjectActor(UObject* Object, AActor*& Actor) {
}


