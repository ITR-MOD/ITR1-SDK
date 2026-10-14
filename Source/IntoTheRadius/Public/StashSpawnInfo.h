#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ActorSpawnInfo.h"
#include "StashSpawnInfo.generated.h"

class UPRItemConfig;

USTRUCT(BlueprintType)
struct INTOTHERADIUS_API FStashSpawnInfo : public FActorSpawnInfo {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float SpawnChange;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FString InstanceId;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    int32 LoopPoints;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FGameplayTag LootTable;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<UPRItemConfig*> LootConfigs;
    
    FStashSpawnInfo();
};

