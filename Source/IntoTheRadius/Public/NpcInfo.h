#pragma once
#include "CoreMinimal.h"
#include "NpcInfo.generated.h"

class ACharacter;

USTRUCT(BlueprintType)
struct FNpcInfo {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    ACharacter* CharacterRef;
    
    INTOTHERADIUS_API FNpcInfo();
};

