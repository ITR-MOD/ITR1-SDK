#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IAttack.generated.h"

class AActor;
class USceneComponent;

UINTERFACE(Blueprintable, MinimalAPI)
class UIAttack : public UInterface {
    GENERATED_BODY()
};

class IIAttack : public IInterface {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void StopAttack();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void StartAttack();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void SetWeaponVisible(bool bVisible);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void Reload();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void OwnerInitialised();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void OnOutOfAmmo();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void OnMeleeAttackHit();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool NeedWaitForFinish() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool IsReloading() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool IsReadyToAttack();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool IsAttackValid();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool IsAttacking() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    AActor* GetWeaponActor();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    USceneComponent* GetFirePointComponent();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    float GetAttackDistance();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void DestroyWeapon();
    
};

