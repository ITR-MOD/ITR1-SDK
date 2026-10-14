#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "MovementSettingsStance.h"
#include "MovementSettingsState.generated.h"

USTRUCT(BlueprintType)
struct INTOTHERADIUS_API FMovementSettingsState : public FTableRowBase {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FMovementSettingsStance VelocityDirection;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FMovementSettingsStance LookingDirection;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FMovementSettingsStance Aiming;
    
    FMovementSettingsState();
};

