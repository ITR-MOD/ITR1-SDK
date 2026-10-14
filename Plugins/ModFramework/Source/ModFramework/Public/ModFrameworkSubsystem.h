#pragma once
#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "ModFrameworkSubsystem.generated.h"

UCLASS(Blueprintable)
class MODFRAMEWORK_API UModFrameworkSubsystem : public UEngineSubsystem {
    GENERATED_BODY()
public:
    UModFrameworkSubsystem();
};

