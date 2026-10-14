#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "FLMissionData.generated.h"

UCLASS(Blueprintable)
class INTOTHERADIUS_API UFLMissionData : public UBlueprintFunctionLibrary {
    GENERATED_BODY()
public:
    UFLMissionData();

    UFUNCTION(BlueprintCallable)
    static void SetString(const FGameplayTag MissionConfigId, const FString& Key, const FString& Value);
    
    UFUNCTION(BlueprintCallable)
    static void SetInt(const FGameplayTag MissionConfigId, const FString& Key, int32 Value);
    
    UFUNCTION(BlueprintCallable)
    static void SetFloat(const FGameplayTag MissionConfigId, const FString& Key, float Value);
    
    UFUNCTION(BlueprintCallable)
    static void SetBool(const FGameplayTag MissionConfigId, const FString& Key, bool Value);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static void GetString(const FGameplayTag MissionConfigId, const FString& Key, FString& Value, bool& IsSet);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static void GetInt(const FGameplayTag MissionConfigId, const FString& Key, int32& Value, bool& IsSet);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static void GetFloat(const FGameplayTag MissionConfigId, const FString& Key, float& Value, bool& IsSet);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static void GetBool(const FGameplayTag MissionConfigId, const FString& Key, bool& Value, bool& IsSet);
    
    UFUNCTION(BlueprintCallable)
    static void CleanUp(const FGameplayTag MissionConfigId);
    
};

