#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "PRCondition_NPC.h"
#include "PRCondition_NPCKilledWeapon.generated.h"

UCLASS(Blueprintable, EditInlineNew)
class INTOTHERADIUS_API UPRCondition_NPCKilledWeapon : public UPRCondition_NPC {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FGameplayTag WeaponTypeID;
    
    UPRCondition_NPCKilledWeapon();

};

