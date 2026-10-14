#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DebugAndCheats.generated.h"

class APawn;

UINTERFACE(Blueprintable, MinimalAPI)
class UDebugAndCheats : public UInterface {
    GENERATED_BODY()
};

class IDebugAndCheats : public IInterface {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void SwitchCheatInvisibility();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void SwitchCheatGodMode();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void SetDebugAIPawn(APawn* Pawn);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void SetCheatInvisibility(bool bValue);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void SetCheatGodMode(bool bValue);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void GetCheatSpeed(bool& Value);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    bool GetCheatInvisibility();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    bool GetCheatGodMode();
    
};

