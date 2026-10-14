#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RelevanceManager.generated.h"

UCLASS(Blueprintable)
class INTOTHERADIUS_API URelevanceManager : public UObject {
    GENERATED_BODY()
public:
    URelevanceManager();

private:
    UFUNCTION(BlueprintCallable)
    void OnLevelUnloaded();
    
};

