#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CheatsControllerInterface.generated.h"

UINTERFACE(Blueprintable)
class UCheatsControllerInterface : public UInterface {
    GENERATED_BODY()
};

class ICheatsControllerInterface : public IInterface {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool SwitchPlayerCollisions() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool SwitchCheatShowStashes();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool SwitchCheatShowCorpses();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool SwitchCheatShowArtifacts();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void SwitchCheatNoClip();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool SwitchCheatHasSupporterPack();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void SetCheatScreenshot(const bool bValue);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void SetCheatReload(const bool bValue);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void SetCheatPause(const bool bValue);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void SetCheatNoRandom(const bool bValue);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void SetCheatHasSupporterPack(const bool bValue);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool GetCheatShowStashes() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool GetCheatShowCorpses() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool GetCheatShowArtifacts() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool GetCheatScreenshot() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool GetCheatReload() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool GetCheatPause() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool GetCheatNoRandom() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool GetCheatNoClip() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool GetCheatHasSupporterPack() const;
    
};

