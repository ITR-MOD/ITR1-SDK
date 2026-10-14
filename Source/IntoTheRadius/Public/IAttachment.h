#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IAttachment.generated.h"

class UAttachmentSlotComponent;

UINTERFACE(Blueprintable, MinimalAPI)
class UIAttachment : public UInterface {
    GENERATED_BODY()
};

class IIAttachment : public IInterface {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool IsAttached();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void DetachFrom(UAttachmentSlotComponent* SlotComponent);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void AttachTo(UAttachmentSlotComponent* SlotComponent, const bool bMakeSound);
    
};

