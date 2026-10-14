#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "ELevelManagerState.h"
#include "IClearable.h"
#include "IPreparable.h"
#include "LevelManager.generated.h"

class ULevelManager;

UCLASS(Blueprintable)
class INTOTHERADIUS_API ULevelManager : public UObject, public IIPreparable, public IIClearable {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    ELevelManagerState State;
    
    ULevelManager();

    UFUNCTION(BlueprintCallable)
    void UnloadSublevel(const FName& SublevelName);
    
    UFUNCTION(BlueprintCallable)
    void UnloadLevel(const FGameplayTag LevelTag);
    
    UFUNCTION(BlueprintCallable)
    void UnloadAll();
    
    UFUNCTION(BlueprintCallable)
    void ShowSublevel(const FName& SublevelName);
    
private:
    UFUNCTION(BlueprintCallable)
    void OnUnloadingTick();
    
    UFUNCTION(BlueprintCallable)
    void OnLoadingTick();
    
public:
    UFUNCTION(BlueprintCallable)
    void LoadSublevel(const FName& SublevelName);
    
    UFUNCTION(BlueprintCallable)
    void LoadLevel(const FGameplayTag LevelTag);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsReady() const;
    
    UFUNCTION(BlueprintCallable)
    void HideSublevel(const FName& SublevelName, const bool bKeepLoaded);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static ULevelManager* GetLevelManager();
    

    // Fix for true pure virtual functions not being implemented
};

