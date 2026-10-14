#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FLSystem.generated.h"

UCLASS(Blueprintable)
class INTOTHERADIUS_API UFLSystem : public UBlueprintFunctionLibrary {
    GENERATED_BODY()
public:
    UFLSystem();

    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsSimulate();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsShippingBuild();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsQABuild();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsModKitBuild();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsEditor();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsDevelopmentBuild();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsBuild();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FString GetRelevantLanguage();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static void GetPuTimers(float& RawFrameTime, float& RawGameThreadTime, float& RawrenderThreadTime, float& RawGPUFrameTime);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FString GetProjectVersion();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FString GetProjectRevision();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FString GetProjectBeta();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FString GetInstallID();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FString GetFullVersionString();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FString GetClientVersionString();
    
};

