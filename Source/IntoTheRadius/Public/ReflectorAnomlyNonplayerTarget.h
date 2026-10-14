#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ReflectorAnomlyNonplayerTarget.generated.h"

class AActor;

USTRUCT(Atomic, BlueprintType)
struct INTOTHERADIUS_API FReflectorAnomlyNonplayerTarget {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    AActor* ActorRef;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FVector ActorLocation;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FVector ShootLocation;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FVector ActorVelocity;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float SpinTime;
    
    FReflectorAnomlyNonplayerTarget();
};

