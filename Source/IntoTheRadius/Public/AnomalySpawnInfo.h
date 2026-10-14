#pragma once
#include "CoreMinimal.h"
#include "ActorSpawnInfo.h"
#include "AnomalySpawnInfo.generated.h"

USTRUCT(BlueprintType)
struct INTOTHERADIUS_API FAnomalySpawnInfo : public FActorSpawnInfo {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FString CustomID;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FString Name;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float Radius;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool ProjectOnGround;
    
    FAnomalySpawnInfo();
};

