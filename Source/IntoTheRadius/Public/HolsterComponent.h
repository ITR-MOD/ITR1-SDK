#pragma once
#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "GameplayTagContainer.h"
#include "EInventoryMode.h"
#include "IItemHolder.h"
#include "HolsterComponent.generated.h"

class AActor;

UCLASS(Blueprintable, ClassGroup=Custom, meta=(BlueprintSpawnableComponent))
class INTOTHERADIUS_API UHolsterComponent : public USceneComponent, public IIItemHolder {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FGameplayTag ID;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    AActor* Holds;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float WorkDistance;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    int32 InventoryModes;
    
    UHolsterComponent(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable, BlueprintPure)
    void CheckInteractionMode(const EInventoryMode Mode, bool& allow) const;
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void CalcDistance(AActor* targetActor, bool& bInRange, float& Distance);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void AttachAndLerpHold();
    

    // Fix for true pure virtual functions not being implemented
};

