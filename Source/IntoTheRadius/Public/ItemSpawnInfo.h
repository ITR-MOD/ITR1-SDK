#pragma once
#include "CoreMinimal.h"
#include "ActorSpawnInfo.h"
#include "ItemSpawnInfo.generated.h"

USTRUCT(BlueprintType)
struct INTOTHERADIUS_API FItemSpawnInfo : public FActorSpawnInfo {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FString SpawnerId;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float LootPossibilityOverride;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool ProjectOnGround;
    
    FItemSpawnInfo();
};

