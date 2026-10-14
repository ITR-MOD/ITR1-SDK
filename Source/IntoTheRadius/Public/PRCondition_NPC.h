#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "PRCondition.h"
#include "PRCondition_NPC.generated.h"

UCLASS(Abstract, Blueprintable, EditInlineNew)
class INTOTHERADIUS_API UPRCondition_NPC : public UPRCondition {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FGameplayTag NPCConfigID;
    
    UPRCondition_NPC();

};

