#include "MinimapComposeHelper.h"

UMinimapComposeHelper::UMinimapComposeHelper() {
}

void UMinimapComposeHelper::PrepareLandscape(ALandscape* Landscape) {
}

TArray<FIntVector> UMinimapComposeHelper::GetPointsFromFile(const FString& FilePath) {
    return TArray<FIntVector>();
}

TArray<FVector> UMinimapComposeHelper::GetLocations(UInstancedStaticMeshComponent* Component) {
    return TArray<FVector>();
}

ULandscapeSplinesComponent* UMinimapComposeHelper::GetLandscapeSplines(const ALandscape* Landscape) {
    return NULL;
}

void UMinimapComposeHelper::GetHeatmapFromFile(const FString& FilePath, const int32 step, TMap<FIntPoint, int32>& heatmap, FIntPoint& Min, FIntPoint& Max, int32& MaxValue) {
}

TArray<FTransform> UMinimapComposeHelper::GetFoliageTransforms(AInstancedFoliageActor* foliageActor, UFoliageType* FoliageType) {
    return TArray<FTransform>();
}

void UMinimapComposeHelper::GenerateHeatmapFromFile(UObject* WorldContextObject, const FString& FilePath, const int32 step) {
}


