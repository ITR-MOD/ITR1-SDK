#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "IClearable.h"
#include "IPreparable.h"
#include "StringToTagsList.h"
#include "GameInventory.generated.h"

class UInventoryItem;
class UInventoryItemsCollection;
class UOpenedStashIDs;

UCLASS(Blueprintable)
class INTOTHERADIUS_API UGameInventory : public UObject, public IIClearable, public IIPreparable {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    UInventoryItemsCollection* PlayerInventory;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    UInventoryItemsCollection* TutorialPlayerInventory;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<FGameplayTag, UInventoryItemsCollection*> LevelInventories;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    UInventoryItemsCollection* RepairInventory;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<FGameplayTag, UOpenedStashIDs*> OpenedStashes;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FString> SpawnedUniqueItems;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FStringToTagsList> WeaponAmmoBelts;
    
    UGameInventory();

    UFUNCTION(BlueprintCallable)
    void SetAmmoBeltContent(const FString& InstanceId, TArray<FGameplayTag> beltContent);
    
    UFUNCTION(BlueprintCallable)
    UInventoryItem* GetItem(const FString& InstanceId);
    
    UFUNCTION(BlueprintCallable)
    void GetAmmoBeltContent(const FString& InstanceId, TArray<FGameplayTag>& beltContent);
    

    // Fix for true pure virtual functions not being implemented
};

