#pragma once
#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "RadiusGameData.generated.h"

UCLASS(Blueprintable)
class INTOTHERADIUS_API URadiusGameData : public USaveGame {
    GENERATED_BODY()
public:
    URadiusGameData();

};

