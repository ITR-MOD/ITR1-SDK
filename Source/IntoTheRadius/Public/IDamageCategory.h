#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EDamageCategory.h"
#include "IDamageCategory.generated.h"

UINTERFACE(Blueprintable, MinimalAPI)
class UIDamageCategory : public UInterface {
    GENERATED_BODY()
};

class IIDamageCategory : public IInterface {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    EDamageCategory GetDamageCategory() const;
    
};

