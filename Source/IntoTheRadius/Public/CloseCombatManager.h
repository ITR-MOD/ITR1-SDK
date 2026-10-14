#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ENpcCombatState.h"
#include "IClearable.h"
#include "NpcInfo.h"
#include "CloseCombatManager.generated.h"

class ACharacter;
class APRPlayerCharacter;
class UCloseCombatManager;

UCLASS(Blueprintable)
class INTOTHERADIUS_API UCloseCombatManager : public UObject, public IIClearable {
    GENERATED_BODY()
public:
private:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FNpcInfo> ManagedNpcList;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    APRPlayerCharacter* PlayerRef;
    
public:
    UCloseCombatManager();

    UFUNCTION(BlueprintCallable)
    void UnregisterNpc(ACharacter* NpcToUnregister);
    
    UFUNCTION(BlueprintCallable)
    void ToggleDebug();
    
    UFUNCTION(BlueprintCallable)
    void RegisterNpc(ACharacter* NpcToRegister);
    
private:
    UFUNCTION(BlueprintCallable)
    void InitData();
    
public:
    UFUNCTION(BlueprintCallable, BlueprintPure)
    float GetTeleportSearchRadius() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    float GetMinDistanceFromPlayerToReappear() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool GetDebugEnabled() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    ENpcCombatState GetCloseCombatState(ACharacter* NPC) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static UCloseCombatManager* GetCloseCombatManager();
    
    UFUNCTION(BlueprintCallable)
    void DebugSetBudget(const float NewBudget);
    
    UFUNCTION(BlueprintCallable)
    void DebugGetBudget(float& CCBudget, float& Weight);
    
private:
    UFUNCTION(BlueprintCallable)
    void CleanData();
    

    // Fix for true pure virtual functions not being implemented
};

