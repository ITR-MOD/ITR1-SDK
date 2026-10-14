#include "MapPointsConfig.h"

UMapPointsConfig::UMapPointsConfig() {
}

FTransform UMapPointsConfig::WorldToLevelTransform(FGameplayTag LevelTag, FTransform Transform) {
    return FTransform{};
}

FVector UMapPointsConfig::WorldToLevelLocation(FGameplayTag LevelTag, FVector Location) {
    return FVector{};
}

void UMapPointsConfig::ResolvePoint(FGameplayTag pointTag, TArray<FGameplayTag> ExcludedPoints, FGameplayTag& ResultPoint, FGameplayTag& ResultLevel) {
}

FTransform UMapPointsConfig::LevelToWorldTransform(FGameplayTag LevelTag, FTransform Transform) {
    return FTransform{};
}

FVector UMapPointsConfig::LevelToWorldLocation(FGameplayTag LevelTag, FVector Location) {
    return FVector{};
}

bool UMapPointsConfig::IsPointInLevel(FGameplayTag pointTag, FGameplayTag LevelTag) {
    return false;
}

bool UMapPointsConfig::IsPointACluster(FGameplayTag pointTag) {
    return false;
}

void UMapPointsConfig::GetPointTransforms(FGameplayTag pointTag, bool& bSuccess, FGameplayTag& LevelTag, FTransform& RelativeTransform, FTransform& absoluteTransform) {
}

FGameplayTag UMapPointsConfig::GetPointLevel(FGameplayTag pointTag) const {
    return FGameplayTag{};
}

UMapPointsConfig* UMapPointsConfig::GetMapPointsConfig() {
    return NULL;
}

TArray<FGateInfo> UMapPointsConfig::GetLevelGateInfos(const FGameplayTag LevelTag) {
    return TArray<FGateInfo>();
}


