#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "DamageDealerInterface.generated.h"

UINTERFACE(Blueprintable, MinimalAPI)
class UDamageDealerInterface : public UInterface {
    GENERATED_BODY()
};

class IDamageDealerInterface : public IInterface {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    FGameplayTag GetDamageDealerTag();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    FText GetDamageDealerName();
    
};

