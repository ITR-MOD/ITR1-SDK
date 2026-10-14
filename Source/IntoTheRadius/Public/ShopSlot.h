#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "PRBasicActor.h"
#include "ShopSlot.generated.h"

class AActor;
class USceneComponent;

UCLASS(Abstract, Blueprintable)
class INTOTHERADIUS_API AShopSlot : public APRBasicActor {
    GENERATED_BODY()
public:
    AShopSlot(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void UpdateSlot(USceneComponent* componentToReturnActorTo, const FGameplayTag& GameplayTag, AActor* actorToAttach);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void SetLock(bool bLock, bool bAnnounce);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void Clear();
    
};

