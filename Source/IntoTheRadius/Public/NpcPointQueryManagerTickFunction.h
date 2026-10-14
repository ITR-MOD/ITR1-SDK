#pragma once
#include "CoreMinimal.h"
#include "Engine/EngineBaseTypes.h"
#include "NpcPointQueryManagerTickFunction.generated.h"

USTRUCT(BlueprintType)
struct FNpcPointQueryManagerTickFunction : public FTickFunction {
    GENERATED_BODY()
public:
    INTOTHERADIUS_API FNpcPointQueryManagerTickFunction();
};

template<>
struct TStructOpsTypeTraits<FNpcPointQueryManagerTickFunction> : public TStructOpsTypeTraitsBase2<FNpcPointQueryManagerTickFunction>
{
    enum
    {
        WithCopy = false
    };
};

