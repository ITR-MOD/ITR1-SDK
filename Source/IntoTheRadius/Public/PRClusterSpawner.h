#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "IInstanceID.h"
#include "MinMaxInt.h"
#include "PRBasicActor.h"
#include "Templates/SubclassOf.h"
#include "PRClusterSpawner.generated.h"

class APRClusterPattern;
class UPRItemConfig;

UCLASS(Blueprintable)
class INTOTHERADIUS_API APRClusterSpawner : public APRBasicActor, public IIInstanceID {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FGameplayTag ClusterID;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<TSubclassOf<APRClusterPattern>> Patterns;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool ProjectAnomalies;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FGameplayTag> DefaultAtrifacts;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FMinMaxInt DefaultAtrifactsCount;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    TArray<UPRItemConfig*> MissionAtrifacts;
    
    APRClusterSpawner(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable)
    void OnCheckTick();
    

    // Fix for true pure virtual functions not being implemented
};

