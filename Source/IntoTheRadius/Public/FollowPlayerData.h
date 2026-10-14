#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FollowPlayerData.generated.h"

USTRUCT(BlueprintType)
struct FFollowPlayerData {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool bFollowPlayer;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float PlayerFollowVelocityModifier;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FVector PlayerFollowOffset;
    
    INTOTHERADIUS_API FFollowPlayerData();
};

