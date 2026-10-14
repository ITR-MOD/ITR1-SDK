#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "IItemHolder.generated.h"

class AActor;

UINTERFACE(Blueprintable)
class UIItemHolder : public UInterface {
    GENERATED_BODY()
};

class IIItemHolder : public IInterface {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void SpawnContent();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void SetHolderVisibility(bool Visible);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void SetContentVisibility(bool Visible);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void RefreshHolder();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void IsSnapHolder(bool& snapHolder);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void IsAcceptItemActor(AActor* itemActor, bool onlyIfFree, bool& accept);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void GetHolderID(FGameplayTag& HolderID);
    
};

