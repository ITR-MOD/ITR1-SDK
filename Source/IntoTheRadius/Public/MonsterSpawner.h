#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "IInstanceID.h"
#include "NPCGameData.h"
#include "OneParamMonsterSpawnerDelegate.h"
#include "PRBasicActor.h"
#include "MonsterSpawner.generated.h"

class AActor;
class UObject;
class USphereComponent;

UCLASS(Abstract, Blueprintable)
class INTOTHERADIUS_API AMonsterSpawner : public APRBasicActor, public IIInstanceID {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOneParamMonsterSpawner OnWildSpawnerCleared;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    USphereComponent* Sphere;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool Wild;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FString SpawnerInstanceID;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool bForceInstanceID;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float Radius;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<FGameplayTag, int32> Monsters;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool CanWalkAway;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool SnipingMode;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    int32 AlternativeMonstersMax;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FGameplayTag KillMissionPointTag;
    
private:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FNPCGameData> SavedNPCsToSpawn;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<AActor*> NPCActors;
    
public:
    AMonsterSpawner(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObj"))
    static void ToggleNpcSummary(UObject* WorldContextObj);
    
    UFUNCTION(BlueprintCallable)
    void SpawnSavedNPC(bool& bSuccess, AActor*& NPCActor);
    
    UFUNCTION(BlueprintCallable)
    void SpawnNPC(bool& bSuccess, AActor*& NPCActor);
    
private:
    UFUNCTION(BlueprintCallable)
    void OnNPCDied(AActor* NPCActor);
    
public:
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static TMap<FGameplayTag, int32> NPCsListToMonstersMap(const TArray<FGameplayTag>& List);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsNpcSummaryEnabled();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsFull();
    

    // Fix for true pure virtual functions not being implemented
};

