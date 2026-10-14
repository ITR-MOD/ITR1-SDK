#pragma once
#include "CoreMinimal.h"
#include "MovementSettings.h"
#include "MovementSettingsStance.generated.h"

USTRUCT(BlueprintType)
struct INTOTHERADIUS_API FMovementSettingsStance {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FMovementSettings Standing;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FMovementSettings Crouching;
    
    FMovementSettingsStance();
};

