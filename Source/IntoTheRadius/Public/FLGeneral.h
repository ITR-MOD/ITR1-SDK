#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/NoExportTypes.h"
#include "UObject/NoExportTypes.h"
#include "UObject/NoExportTypes.h"
#include "UObject/NoExportTypes.h"
#include "UObject/NoExportTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/EngineTypes.h"
#include "GameplayTagContainer.h"
#include "GameplayTagContainer.h"
#include "AmmoContainerData.h"
#include "EDifficultyType.h"
#include "EItemsDropType.h"
#include "EPRGameDifficultyPreset.h"
#include "ETracersShowType.h"
#include "MinMaxFloat.h"
#include "MinMaxInt.h"
#include "PRNPCInfo.h"
#include "Parallelepiped.h"
#include "RadiusGameDifficulty.h"
#include "Templates/SubclassOf.h"
#include "TutorialConfig.h"
#include "FLGeneral.generated.h"

class AActor;
class ACharacter;
class APawn;
class UActorComponent;
class UCharacterMovementComponent;
class UDamageType;
class UInventoryItem;
class ULevel;
class ULightComponent;
class UObject;
class UPRCondition;
class UPhysicsConstraintComponent;
class UPrimitiveComponent;
class USceneComponent;
class USplineComponent;

UCLASS(Blueprintable)
class INTOTHERADIUS_API UFLGeneral : public UBlueprintFunctionLibrary {
    GENERATED_BODY()
public:
    UFLGeneral();

    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FText TracersShowTypeToText(const ETracersShowType TracersShowType);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FString TracersShowTypeToString(const ETracersShowType TracersShowType);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FString TagsArrayAsString(const FString& Prefix, TArray<FGameplayTag> tagsArray);
    
    UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
    static AActor* SpawnActorWithLabel(const UObject* WorldContextObject, const TSubclassOf<AActor> ActorClass, const FVector& Location, const FString& Label);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static void SortComponentsByLabel(TArray<USceneComponent*> In, TArray<USceneComponent*>& Out);
    
    UFUNCTION(BlueprintCallable)
    static void ShowMessage(const FString& Message, const FString& Title);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FTransform ShiftActorByComponent(const AActor* Actor, const USceneComponent* Component, const FTransform& TargetTransform);
    
    UFUNCTION(BlueprintCallable)
    static void SetLightVisibility(ULightComponent* Light, bool IsVisible);
    
    UFUNCTION(BlueprintCallable)
    static void SetLightAffectsWorld(ULightComponent* Component, bool bAffects);
    
    UFUNCTION(BlueprintCallable)
    static void SetCanEverAffectNavigation(UActorComponent* Component, bool bRelevant);
    
    UFUNCTION(BlueprintCallable)
    static void SetAngularRotationOffset(UPhysicsConstraintComponent* Component, const FRotator RotationOffset);
    
    UFUNCTION(BlueprintCallable)
    static void SetActorSimulatePhysics(const AActor* Actor, bool Simulate, bool zeroVel, bool& success, UPrimitiveComponent*& Primitive);
    
    UFUNCTION(BlueprintCallable)
    static bool SetActorEnableGravity(const AActor* Actor, bool enable);
    
    UFUNCTION(BlueprintCallable)
    static void SafeDestroyActor(AActor* Actor);
    
    UFUNCTION(BlueprintCallable)
    static FGameplayTagContainer RequestGameplayTagChildren(const TArray<FGameplayTag>& GameplayTags);
    
    UFUNCTION(BlueprintCallable)
    static void RefreshHolders(UObject* Object);
    
    UFUNCTION(BlueprintCallable)
    static void PutActorInPersistentLevel(AActor* Actor);
    
    UFUNCTION(BlueprintCallable)
    static float ProtectedApplyPointDamage(AActor* DamagedActor, float BaseDamage, const FVector& HitFromDirection, const FHitResult& HitInfo, AActor* ActorInstigator, AActor* DamageCauser, TSubclassOf<UDamageType> DamageTypeClass);
    
    UFUNCTION(BlueprintCallable)
    static void PrintObjectReferences(UObject* Object);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FRadiusGameDifficulty PresetToDifficulty(const EPRGameDifficultyPreset DifficultyPreset);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool ParseCommandLine(const FString& ParameterName, FString& Value);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FMinMaxFloat Multiply_MinMaxFloat(FMinMaxFloat A, const float B);
    
    UFUNCTION(BlueprintCallable)
    static void KillActorWithChildren(AActor* Actor);
    
    UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
    static void KillActorsByClass(const UObject* WorldContextObject, TSubclassOf<AActor> ActorsClass, bool bWithChildren);
    
    UFUNCTION(BlueprintCallable)
    static void KillActor(AActor* Actor);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FText ItemsDropTypeToText(const EItemsDropType DropType);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FString ItemsDropTypeToString(const EItemsDropType DropType);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsZeroParallelepiped(FParallelepiped Rect);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsZeroInt(FMinMaxInt MinMax);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsZeroFloat(FMinMaxFloat MinMax);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsZeroDiffInt(FMinMaxInt MinMax);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsZeroDiffFloat(FMinMaxFloat MinMax);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsProfilingInCommandLine();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsPreviewActor(const AActor* Actor);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsPassedTutorialUnseen(FGameplayTag TriggerTag);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsParallelepipedIntersectSphere(const FParallelepiped Rect, const FVector SphereCenter, const float SphereRadius);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsParallelepipedIntersectPoint(const FParallelepiped Rect, const FVector Point);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsNecessaryTutorial();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsInTutorialLevel();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsInMainMenuLevel();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsInBoundsIgnoreZ(const FVector& Point, const FTransform& BoxWorldTransform, const FVector& BoxExtent);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsGameStarted();
    
    UFUNCTION(BlueprintCallable, BlueprintPure, meta=(WorldContext="WorldContextObject"))
    static bool IsGameRunning(const UObject* WorldContextObject);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsGameplayRunning();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsGameClosing();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsFreeGame();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsEditorCustomStartup();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsEarlyAccessPlayer();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsDifficultyChanged(const FRadiusGameDifficulty NewDifficulty);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsDifficultyAppliedAfterTide(const FRadiusGameDifficulty NewDifficulty);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsDefaultUpgrade(const FGameplayTag UpgradeID, UInventoryItem* Item);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsCorpsesCensored();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsConditionsPassed(const TArray<UPRCondition*> Conditions);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsComponentBoundsIntersect(const UPrimitiveComponent* ComponentA, const UPrimitiveComponent* ComponentB);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsCommandLine(const FString& ParameterName);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsActorInLevel(const AActor* Actor, const FGameplayTag LevelTag);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsActorInCurrentLevel(const AActor* Actor);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool HasUnseenPassedTutorials();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool HasPassedTutorials();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static float GetTrueMin(FMinMaxFloat MinMax);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static float GetTrueMax(FMinMaxFloat MinMax);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static void GetTimeStringFromSeconds(int32 TotalSeconds, bool bNoSeconds, FString& String);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static void GetTimeSpentString(int32 TotalMinutes, FString& String);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FName GetStreamingLevelNameFromActor(const AActor* Actor);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static float GetSignedAngle(FVector Normal, FVector v1, FVector v2);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FTransform GetRelativeTransform(const AActor* Actor);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static int32 GetRandomInt(FMinMaxInt MinMax);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static float GetRandomFloat(FMinMaxFloat MinMax);
    
    UFUNCTION(BlueprintCallable, BlueprintPure, meta=(WorldContext="WorldContextObject"))
    static void GetPRPlayerPawn(const UObject* WorldContextObject, APawn*& prPlayerPawn);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FVector GetPointsBoxSize(const TArray<FVector> Points);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static UClass* GetParentClass(UClass* Class);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static UClass* GetObjectParentClass(UObject* Object);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FName GetObjectFName(const UObject* Object);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FString GetNewNoteMarker(const FGameplayTag& Tag);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FTutorialConfig GetNecessaryTutorialConfig();
    
    UFUNCTION(BlueprintCallable, BlueprintPure, meta=(WorldContext="WorldContextObject"))
    static void GetMapLevelTag(const UObject* WorldContextObject, FGameplayTag& LevelTag);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static void GetLevelByName(const FString& LevelName, FGameplayTag& LevelTag);
    
    UFUNCTION(BlueprintCallable)
    static FBox GetLevelBounds(ULevel* TargetLevel, FVector& LevelLocation);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool GetGamePurchaseDate(FDateTime& PurchaseDate, FString& PurchaseDateString);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static void GetDateTimeString(FDateTime DateTime, FString& String);
    
    UFUNCTION(BlueprintCallable)
    static UObject* GetAttackFromPawn(APawn* Pawn);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static TArray<FGameplayTag> GetAllSecondaryWeaponTags();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static TArray<FGameplayTag> GetAllPrimaryWeaponTags();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static TArray<FGameplayTag> GetAllCassetteTags();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static TArray<FGameplayTag> GetAllArtifactTags();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FGameplayTag GetActorLevelTag(const AActor* Actor);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FString GetActorLevelName(const AActor* Actor);
    
    UFUNCTION(BlueprintCallable)
    static void GetActorHolderTransform(const AActor* Actor, const FGameplayTag HolderID, bool& success, FTransform& RelativeTransform);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FString GameDefaultMap();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static int32 FindIndexOfTag(const FGameplayTag& Tag, const TArray<FGameplayTag>& Array, bool bExact);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static float FindDistanceAlongSplineClosestToLocation(const USplineComponent* Spline, const FVector& Location);
    
    UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
    static void DrawArrow(const UObject* WorldContextObject, const FVector LineStart, const FVector LineEnd, const float ArrowSize, const FLinearColor Color, const float Duration, const float Thickness);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FText DifficultyTypeToText(const EDifficultyType DifficultyType);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FString DifficultyTypeToString(const EDifficultyType DifficultyType);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static EPRGameDifficultyPreset DifficultyToPreset(const FRadiusGameDifficulty GameDifficulty);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FText DifficultyPresetToText(const EPRGameDifficultyPreset DifficultyPreset);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FString DifficultyPresetToString(const EPRGameDifficultyPreset DifficultyPreset);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static void DifficultiesToNames(TArray<EDifficultyType> Difficulties, TArray<FText>& Names, TArray<int32>& Indexes);
    
    UFUNCTION(BlueprintCallable)
    static void ClearUnseenPassedTutorials();
    
    UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
    static void CheckUniqueInstanceID(const UObject* WorldContextObject, UPARAM(Ref) FString& UID, const FString& Prefix, const bool bOnlyIfEmpty);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FVector CalcLocalBounds(USceneComponent* Component);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool BlinkFloorSweepTest(UCharacterMovementComponent* MovementComponent, ACharacter* CharacterOwner, FHitResult& Hit, const FVector& CapsuleLocation, const FVector& TraceStart, const FVector& TraceEnd);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static TArray<FGameplayTag> AsArray(const FGameplayTagContainer& Container);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FPRNPCInfo ApplyDifficultyModifiers(FPRNPCInfo NpcInfo);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FString AmmoContainerDataAsString(const FString& Prefix, FAmmoContainerData AmmoContainerData);
    
    UFUNCTION(BlueprintCallable)
    static UActorComponent* AddComponentToActor(AActor* BaseActor, TSubclassOf<UActorComponent> AttachedComponent);
    
};

