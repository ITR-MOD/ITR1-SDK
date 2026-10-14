#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RadiusGameDifficulty.h"
#include "RadiusGameInstanceInterface.generated.h"

UINTERFACE(Blueprintable)
class URadiusGameInstanceInterface : public UInterface {
    GENERATED_BODY()
};

class IRadiusGameInstanceInterface : public IInterface {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void StartNewGame(const int32 ProfileNumber, const bool bSkipTutorial, const bool bIronman, const FRadiusGameDifficulty GameDifficulty, const bool bFromMainMenu);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void LoadGame(const int32 SlotNr, const int32 SaveNr);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void ExitGame(const bool bMenu, const bool bSave);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void ContinueLastGameInSlot(const int32 SlotNr);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void ContinueLastGame();
    
};

