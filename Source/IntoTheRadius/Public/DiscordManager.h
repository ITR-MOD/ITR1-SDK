#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "IClearable.h"
#include "DiscordManager.generated.h"

class UDiscordManager;

UCLASS(Blueprintable)
class INTOTHERADIUS_API UDiscordManager : public UObject, public IIClearable {
    GENERATED_BODY()
public:
    UDiscordManager();

    UFUNCTION(BlueprintCallable)
    void Initialize();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static UDiscordManager* GetDiscordManager();
    
    UFUNCTION(BlueprintCallable)
    void ChangeLocationName();
    
    UFUNCTION(BlueprintCallable)
    void ChangeDifficulty();
    

    // Fix for true pure virtual functions not being implemented
};

