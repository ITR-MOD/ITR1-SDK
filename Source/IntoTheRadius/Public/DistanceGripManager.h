#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DistanceGripCandidate.h"
#include "DistanceGripManager.generated.h"

class APRPlayerCharacter;
class UInventoryItem;

UCLASS(Blueprintable)
class INTOTHERADIUS_API UDistanceGripManager : public UObject {
    GENERATED_BODY()
public:
private:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    APRPlayerCharacter* GripSystemOwner;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<UInventoryItem*, FDistanceGripCandidate> CandidatePool;
    
public:
    UDistanceGripManager();

private:
    UFUNCTION(BlueprintCallable)
    void LevelTransitionStartCallback();
    
    UFUNCTION(BlueprintCallable)
    void LevelTransitionEndCallback();
    
    UFUNCTION(BlueprintCallable)
    void ItemTakenFromLevelCallback(UInventoryItem* ItemTaken);
    
    UFUNCTION(BlueprintCallable)
    void ItemStoredToLevelCallback(UInventoryItem* ItemStored);
    
    UFUNCTION(BlueprintCallable)
    void ItemRemovedFromLevelCallback(UInventoryItem* ItemRemoved);
    
    UFUNCTION(BlueprintCallable)
    void ItemActorRemovedCallback(UInventoryItem* ItemWithActorRemoved);
    
};

