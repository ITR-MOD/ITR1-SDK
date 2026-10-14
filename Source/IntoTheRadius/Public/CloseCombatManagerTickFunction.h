#pragma once
#include "CoreMinimal.h"
#include "Engine/EngineBaseTypes.h"
#include "CloseCombatManagerTickFunction.generated.h"

USTRUCT(BlueprintType)
struct FCloseCombatManagerTickFunction : public FTickFunction {
    GENERATED_BODY()
public:
    INTOTHERADIUS_API FCloseCombatManagerTickFunction();
};

template<>
struct TStructOpsTypeTraits<FCloseCombatManagerTickFunction> : public TStructOpsTypeTraitsBase2<FCloseCombatManagerTickFunction>
{
    enum
    {
        WithCopy = false
    };
};

