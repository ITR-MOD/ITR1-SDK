#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "PRCondition.h"
#include "PRCondition_Item.generated.h"

UCLASS(Abstract, Blueprintable, EditInlineNew)
class INTOTHERADIUS_API UPRCondition_Item : public UPRCondition {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FGameplayTag ItemConfigID;
    
    UPRCondition_Item();

};

