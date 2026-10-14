#pragma once
#include "CoreMinimal.h"
#include "EPRPlatformType.h"
#include "FollowPlayerData.h"
#include "PRBasicActor.h"
#include "PRAsh.generated.h"

UCLASS(Blueprintable)
class INTOTHERADIUS_API APRAsh : public APRBasicActor {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<EPRPlatformType, FFollowPlayerData> FollowPlayerDataMap;
    
    APRAsh(const FObjectInitializer& ObjectInitializer);

};

