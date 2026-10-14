#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/NoExportTypes.h"
#include "UObject/NoExportTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EPRHandPoseType.h"
#include "FLProject.generated.h"

class AActor;
class APawn;
class UGripMotionControllerComponent;
class UObject;

UCLASS(Blueprintable)
class INTOTHERADIUS_API UFLProject : public UBlueprintFunctionLibrary {
    GENERATED_BODY()
public:
    UFLProject();

    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FString TrimFloat(const float In, const int32 Digits);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FVector RoundVector(const FVector& In, const int32 Digits);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FTransform RoundTransform(const FTransform& In);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FRotator RoundRotator(const FRotator& In, const int32 Digits);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static float RoundFloat(const float In, const int32 Digits);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static float Negative(const float& In);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    TArray<UObject*> NativeSortObjectsByDistance(const TArray<UObject*> inArray, const AActor* DistanceFromActor);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsInputEnabledAndNotSimulation();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool GetSpawnPoint(const FVector& nearby, const float InRadius, const bool reachable, FVector& OutPoint);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool GetReachablePointNotVisibleToPlayer(const APawn* PlayerPawn, const APawn* NPC, float MinDistFromPlayer, float SearchRadius, int32 NumAttempts, FVector& OutPoint);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool GetNavigablePointBehindPlayer(const APawn* PlayerPawn, float MinDistFromPlayer, float SearchRadius, FVector& OutPoint);
    
    UFUNCTION(BlueprintCallable)
    static bool GetGripRelativeTransform(UObject* GripObject, UGripMotionControllerComponent* HandController, EPRHandPoseType PoseType, FTransform& OutGripRelativeTransform);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static void GetGripObjectActor(UObject* Object, AActor*& Actor);
    
};

