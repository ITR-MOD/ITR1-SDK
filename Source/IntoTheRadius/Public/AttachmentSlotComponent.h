#pragma once
#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "GameplayTagContainer.h"
#include "EItemSlot.h"
#include "AttachmentSlotComponent.generated.h"

class AActor;
class UBoxComponent;
class UInventoryItem;
class UStaticMesh;

UCLASS(Blueprintable, EditInlineNew, ClassGroup=Custom, meta=(BlueprintSpawnableComponent))
class INTOTHERADIUS_API UAttachmentSlotComponent : public UStaticMeshComponent {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    EItemSlot Slot;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FGameplayTag AttachmentType;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FName AttachToSocket;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    UStaticMesh* MyStaticMesh;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    UBoxComponent* BoxCollision;
    
    UAttachmentSlotComponent(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void TryDetachItem(AActor* Attachment, bool& bSuccess);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void TryAttachItem(AActor* Attachment, bool& bSuccess);
    
    UFUNCTION(BlueprintCallable)
    void SetSlotEnabled(const bool bEnable);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsSlotEnabled() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsSlotEmpty() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void IsParentHeld(bool& IsHeld) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void IsAccepts(const FGameplayTag Type, bool& bCanAttach) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    UInventoryItem* GetParentItem() const;
    
};

