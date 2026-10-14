#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/NoExportTypes.h"
#include "ILevelPreparator.generated.h"

UINTERFACE(Blueprintable)
class UILevelPreparator : public UInterface {
    GENERATED_BODY()
};

class IILevelPreparator : public IInterface {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool IsPreparing();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool CheckPreparator(FVector Location);
    
};

