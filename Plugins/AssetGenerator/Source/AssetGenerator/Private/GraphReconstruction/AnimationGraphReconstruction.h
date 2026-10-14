#pragma once
#include "CoreMinimal.h"
#include "UE427AssetPath.h"

class UAnimBlueprint;
class FJsonObject;

namespace AnimationGraphReconstruction
{
    bool Validate(const TSharedPtr<FJsonObject>& Data, FString& Error);
    bool Build(UAnimBlueprint* Blueprint, const TSharedPtr<FJsonObject>& Data, FString& Error);
    bool ValidateRecovered(const TSharedPtr<FJsonObject>& Data, FString& Error);
    bool BuildRecovered(UAnimBlueprint* Blueprint, const TSharedPtr<FJsonObject>& Data, FString& Error);
    bool RestorePropertyAccesses(UAnimBlueprint* Blueprint, const TSharedPtr<FJsonObject>& Data, FString& Error);
}
