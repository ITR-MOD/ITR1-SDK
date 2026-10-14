#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "PRAnomalyInfo.h"
#include "IAnomaly.generated.h"

UINTERFACE(Blueprintable)
class UIAnomaly : public UInterface {
    GENERATED_BODY()
};

class IIAnomaly : public IInterface {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    FGameplayTag GetTypeID() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    float GetRadius() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    float GetBaseDamage() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    FPRAnomalyInfo GetAnomalyInfo() const;
    
};

