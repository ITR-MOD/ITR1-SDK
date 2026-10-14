#pragma once
#include "CoreMinimal.h"
#include "PRCondition.h"
#include "PRCondition_MinimumPlayerLevelCheck.generated.h"

UCLASS(Blueprintable, EditInlineNew)
class INTOTHERADIUS_API UPRCondition_MinimumPlayerLevelCheck : public UPRCondition {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    int32 MinimumPlayerLevel;
    
    UPRCondition_MinimumPlayerLevelCheck();

};

