#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AmmoContainerData.h"
#include "PRAmmoContainerInfo.h"
#include "IAmmoContainer.generated.h"

UINTERFACE(Blueprintable, MinimalAPI)
class UIAmmoContainer : public UInterface {
    GENERATED_BODY()
};

class IIAmmoContainer : public IInterface {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void SetLoadingLock(bool lock);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void OnAmmoRemoved();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void OnAmmoAdded();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool IsLoadingLocked() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void GetAmmoContainerInfo(FPRAmmoContainerInfo& Info);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void GetAmmoContainerData(FAmmoContainerData& Data);
    
};

