#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "IInstanceID.h"
#include "PRBasicActor.h"
#include "SpawnerGenerator.generated.h"

class AActor;
class AMonsterSpawner;
class ASpawnerGeneratorAlternativePlace;

UCLASS(Abstract, Blueprintable)
class INTOTHERADIUS_API ASpawnerGenerator : public APRBasicActor, public IIInstanceID {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FGameplayTag generatorID;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<AMonsterSpawner*> Spawners;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float CooldownSeconds;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float HitPoints;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool ForceSpecial;
    
protected:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<ASpawnerGeneratorAlternativePlace*> AlternativePlaces;
    
public:
    ASpawnerGenerator(const FObjectInitializer& ObjectInitializer);

protected:
    UFUNCTION(BlueprintCallable)
    void StopGenerator();
    
public:
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool Special() const;
    
protected:
    UFUNCTION(BlueprintCallable)
    void SaveGeneratorData();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnSpawnerReadyToCall(AMonsterSpawner* Spawner);
    
    UFUNCTION(BlueprintCallable)
    void CallSpawner(AMonsterSpawner* Spawner, bool& bSuccess, AActor*& NPCActor);
    

    // Fix for true pure virtual functions not being implemented
};

