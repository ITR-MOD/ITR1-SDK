#pragma once
#include "CoreMinimal.h"
#include "GrippableStaticMeshActor.h"
#include "IPRItem.h"
#include "BackpackSimple.generated.h"

class APRHolder;
class USceneComponent;

UCLASS(Blueprintable)
class INTOTHERADIUS_API ABackpackSimple : public AGrippableStaticMeshActor, public IIPRItem {
    GENERATED_BODY()
public:
protected:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<APRHolder*> Holders;
    
public:
    ABackpackSimple(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void RefreshState();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnUnequipped();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnReturned();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void LerpToHolster();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, BlueprintPure)
    USceneComponent* GetPivotGripPoint() const;
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    APRHolder* GetHolderSleepingBag();
    
    UFUNCTION(BlueprintCallable)
    TArray<APRHolder*> GetHolders();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    APRHolder* GetHolderPrimaryWeapon();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    APRHolder* GetHolderBackpack();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, BlueprintPure)
    bool CheckEquipped() const;

    // Set whether the grip script can tick or not
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void SetTickEnabled(bool bTickEnabled);
    

    // Fix for true pure virtual functions not being implemented
};

