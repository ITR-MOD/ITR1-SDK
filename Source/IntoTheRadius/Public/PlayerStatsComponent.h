#pragma once
#include "CoreMinimal.h"
#include "EPRStatStatus.h"
#include "IHealthStatus.h"
#include "NoParamsDelegateDelegate.h"
#include "PRBasicActorComponent.h"
#include "StatStatusEventDelegate.h"
#include "PlayerStatsComponent.generated.h"

UCLASS(Blueprintable, ClassGroup=Custom, meta=(BlueprintSpawnableComponent))
class INTOTHERADIUS_API UPlayerStatsComponent : public UPRBasicActorComponent, public IIHealthStatus {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FStatStatusEvent OnHealth;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FStatStatusEvent OnStamina;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FStatStatusEvent OnNutrition;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FNoParamsDelegate OnBlackout;
    
    UPlayerStatsComponent(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void UseStamina(bool Use, float StaminaUseMult);
    
    UFUNCTION(BlueprintCallable)
    void SetStats(const int32 HealthPercent, const int32 StaminaPercent, const int32 RestPercent, const int32 NutritionPercent, const int32 HungerPercent);
    
    UFUNCTION(BlueprintCallable)
    void SetStamina(const float Value);
    
    UFUNCTION(BlueprintCallable)
    void SetHealth(float Health);
    
    UFUNCTION(BlueprintCallable)
    void Reset(bool bAlive);
    
    UFUNCTION(BlueprintCallable)
    void RegenerateHealth(float DeltaSeconds);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool HasStamina();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool HasRest();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool HasNutrition();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static void GetStatus(EPRStatStatus& Health, EPRStatStatus& Stamina, EPRStatStatus& Nutrition);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetStats(float& Health, float& HealthPercent, float& Stamina, float& StaminaPercent, float& Rest, float& RestPercent, float& Nutrition, float& NutritionPercent);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetStaminaPercent(float& StaminaPercent);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetSleepTimeTillRested(const int32 RestPercent, float& Seconds);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetSleepTimeTillHealed(const int32 HealthPercent, float& Seconds);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetRestPercent(float& RestPercent);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetMaxAvailableStamina(float& Stamina, float& MaxAvailable);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetHealthMax(float& HealthMax);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool EnoughStamina(const float Stamina);
    
    UFUNCTION(BlueprintCallable)
    void ChangeStamina(const float Delta);
    
    UFUNCTION(BlueprintCallable)
    void ChangeRestByPercent(const int32 Percent);
    
    UFUNCTION(BlueprintCallable)
    void ChangeRest(const float Delta, const bool bBuff);
    
    UFUNCTION(BlueprintCallable)
    void ChangeNutrition(const float Delta);
    
    UFUNCTION(BlueprintCallable)
    void ChangeHealthByPercent(int32 Percent);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool CanRegenerateHealth();
    
    UFUNCTION(BlueprintCallable)
    void AllowRegeneration(float HealthRegenStart, float HealthRegenMax, float HealthRegenPerSecond);
    

    // Fix for true pure virtual functions not being implemented
};

