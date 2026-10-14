#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "EPRHandPoseType.h"
#include "PRItemInfo.h"
#include "IPRItem.generated.h"

class UHolsterComponent;
class UInventoryItem;

UINTERFACE(Blueprintable)
class UIPRItem : public UInterface {
    GENERATED_BODY()
};

class IIPRItem : public IInterface {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void UpdateSkin();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool UpdateInventoryItemLocation();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void UpdateFireMode();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void UpdateAttachmentSlots();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void StopUse();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void SetupProxySystem(const bool bStartWithProxy, const bool bStartVisible);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void SetProxyMode(const bool bUseProxy);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void SetItemVisibility(const bool bNewVisibility);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void SetDistanceGripEnable(bool enable);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void RegisterMeshComponentsAndOmittableColliders();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void Reconfigure(UInventoryItem* InventoryItem, bool& success);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnRemovedFromHolster(UHolsterComponent* Holster);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnProxySystemReady();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnProxyEnabled();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnProxyDisabled();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnAddedToHolster(UHolsterComponent* Holster);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool IsItemInDelivery() const;
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void InitBought();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    FGameplayTag GetTypeID() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void GetProxyMode(bool& bUseProxy) const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    UInventoryItem* GetPreparedItem() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    FTransform GetPoseShift(EPRHandPoseType PoseType) const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void GetItemVisibility(bool& bIsVisible) const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    FPRItemInfo GetItemInfo() const;
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void GetItemCenterTransform(bool& socket, FTransform& itemCenterTransform);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    UInventoryItem* GetInventoryItem() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    EPRHandPoseType GetDefaultPoseType() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    float GetCurrentWeight() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    float GetCurrentCondition() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    float GetConditionPercent() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    FGameplayTag GetAttachmentTypeID() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void GetAttachmentsList(TArray<FString>& Attachments);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    FString GetActorUID() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool DropItemOnGround();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void DisableItemPhysics();
    
};

