#pragma once
#include "CoreMinimal.h"
#include "EPRPlatformType.h"
#include "Templates/SubclassOf.h"
#include "PoolActorConfig.generated.h"

class AActor;

USTRUCT(Atomic, BlueprintType)
struct INTOTHERADIUS_API FPoolActorConfig {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TSubclassOf<AActor> ActorClass;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<EPRPlatformType> PlatformTypes;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    int32 InitCount;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    int32 MaxCount;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool ReCreateOnDestroy;
    
    FPoolActorConfig();
};

