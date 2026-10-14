#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "PRCondition_IntCheck.h"
#include "PRCondition_StashesOpenedCheck.generated.h"

UCLASS(Blueprintable, EditInlineNew)
class INTOTHERADIUS_API UPRCondition_StashesOpenedCheck : public UPRCondition_IntCheck {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FGameplayTag> StashTypes;
    
    UPRCondition_StashesOpenedCheck();

};

