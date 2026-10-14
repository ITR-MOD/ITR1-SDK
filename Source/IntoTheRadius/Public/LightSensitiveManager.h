#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "LightSensitiveManager.generated.h"

class AActor;
class ULightSensitiveManager;

UCLASS(Blueprintable)
class INTOTHERADIUS_API ULightSensitiveManager : public UObject {
    GENERATED_BODY()
public:
    ULightSensitiveManager();

    UFUNCTION(BlueprintCallable)
    void UnregisterLightSensitiveObject(AActor* LightSensitiveObject);
    
    UFUNCTION(BlueprintCallable)
    void RegisterLightSensitiveObject(AActor* LightSensitiveObject);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static ULightSensitiveManager* GetLightSensitiveManager();
    
};

