#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/NoExportTypes.h"
#include "ActorsArray.h"
#include "IClearable.h"
#include "Templates/SubclassOf.h"
#include "PoolManager.generated.h"

class AActor;
class APawn;
class UPoolManager;

UCLASS(Blueprintable)
class INTOTHERADIUS_API UPoolManager : public UObject, public IIClearable {
    GENERATED_BODY()
public:
private:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<TSubclassOf<AActor>, FActorsArray> Pool;
    
public:
    UPoolManager();

    UFUNCTION(BlueprintCallable)
    bool ReturnActor(AActor* Actor);
    
    UFUNCTION(BlueprintCallable)
    void PrefillPoolFromConfig();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsActorInPool(const AActor* Actor);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static UPoolManager* GetPoolManager();
    
    UFUNCTION(BlueprintCallable)
    AActor* GetActor(const TSubclassOf<AActor> ActorClass, const FTransform Transform, AActor* Owner, APawn* Instigator);
    

    // Fix for true pure virtual functions not being implemented
};

