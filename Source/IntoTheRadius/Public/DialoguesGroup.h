#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Dialogue.h"
#include "DialoguesGroup.generated.h"

class UPRCondition;

USTRUCT(Atomic, BlueprintType)
struct INTOTHERADIUS_API FDialoguesGroup {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FGameplayTag GroupID;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    TArray<UPRCondition*> Conditions;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<FGameplayTag, FDialogue> Cassettes;
    
    FDialoguesGroup();
};

