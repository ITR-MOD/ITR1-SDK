#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/EngineTypes.h"
#include "GameplayTagContainer.h"
#include "AnomalySpawnInfo.h"
#include "ItemSpawnInfo.h"
#include "NpcSpawnInfo.h"
#include "RadiusEmitterInfo.h"
#include "RadiusFirearmShellInfo.h"
#include "RadiusImpactInfo.h"
#include "RadiusNiagaraEmitterInfo.h"
#include "Templates/SubclassOf.h"
#include "FLSpawn.generated.h"

class AActor;
class UInventoryItem;
class UObject;
class USceneComponent;

UCLASS(Blueprintable)
class INTOTHERADIUS_API UFLSpawn : public UBlueprintFunctionLibrary {
    GENERATED_BODY()
public:
    UFLSpawn();

    UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
    static AActor* SpawnNPC(const UObject* WorldContextObject, const FNpcSpawnInfo NpcSpawnInfo);
    
    UFUNCTION(BlueprintCallable)
    static AActor* SpawnNiagaraEmitter(const FTransform Transform, const FRadiusNiagaraEmitterInfo& Info);
    
    UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
    static AActor* SpawnItemByType(const UObject* WorldContextObject, const FGameplayTag& ItemTypeID, const FString& InstanceId, const FTransform& Transform, bool bProjectOnGround, bool bSimulatePhysics);
    
    UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
    static AActor* SpawnItemByTag(const UObject* WorldContextObject, const FGameplayTag Tag, const USceneComponent* ParentComponent, const bool bSnapToParent, const FTransform& Transform, const bool bPreConfigure, const FString& CustomID);
    
    UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
    static AActor* SpawnItemByInventoryItem(const UObject* WorldContextObject, UInventoryItem* InventoryItem, const FTransform& Transform, const bool bProjectOnGround, const bool bSimulatePhysics);
    
    UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
    static AActor* SpawnItem(const UObject* WorldContextObject, const FItemSpawnInfo ItemSpawnInfo);
    
    UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
    static AActor* SpawnInventoryItem(const UObject* WorldContextObject, UInventoryItem* InventoryItem, const USceneComponent* ParentComponent, const bool bKeepRelativeLocation, const bool bSimulatePhysics);
    
    UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
    static AActor* SpawnImpactFromHit(const UObject* WorldContextObject, const FHitResult& Hit, const float ParticleScale, const float DecalScale, const float Distance);
    
    UFUNCTION(BlueprintCallable)
    static AActor* SpawnImpact(const FTransform Transform, const FRadiusImpactInfo& Info);
    
    UFUNCTION(BlueprintCallable)
    static AActor* SpawnEmitter(const FTransform Transform, const FRadiusEmitterInfo& Info);
    
    UFUNCTION(BlueprintCallable)
    static AActor* SpawnBulletShell(const TSubclassOf<AActor> ActorClass, FTransform Transform, const FRadiusFirearmShellInfo ShellInfo);
    
    UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
    static AActor* SpawnAnomaly(const UObject* WorldContextObject, const FAnomalySpawnInfo AnomalySpawnInfo);
    
    UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
    static AActor* SpawnActorWithTransformDeferred(const UObject* WorldContextObject, const TSubclassOf<AActor> ActorClass, const FTransform Transform);
    
    UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
    static AActor* SpawnActorWithIdentityTransform(const UObject* WorldContextObject, const TSubclassOf<AActor> ActorClass, const bool bDeferred);
    
};

