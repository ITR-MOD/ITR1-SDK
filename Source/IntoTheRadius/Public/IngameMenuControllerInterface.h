#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/NoExportTypes.h"
#include "UObject/NoExportTypes.h"
#include "IngameMenuControllerInterface.generated.h"

UINTERFACE(Blueprintable)
class UIngameMenuControllerInterface : public UInterface {
    GENERATED_BODY()
};

class IIngameMenuControllerInterface : public IInterface {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void SwitchIngameMenu();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool IsIngameMenuActive() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void GetPreMenuParameters(FTransform& preMenuTransform, FVector& preMenuVelocity);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void CloseIngameMenu();
    
};

