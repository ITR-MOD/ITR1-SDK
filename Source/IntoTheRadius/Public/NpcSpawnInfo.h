#pragma once
#include "CoreMinimal.h"
#include "ActorSpawnInfo.h"
#include "NpcSpawnInfo.generated.h"

USTRUCT(BlueprintType)
struct INTOTHERADIUS_API FNpcSpawnInfo : public FActorSpawnInfo {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FString CustomID;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FString SpawnerId;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float LootPossibilityOverride;
    
    FNpcSpawnInfo();
};

