#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "PRModificator.generated.h"

class UPRCondition;

UCLASS(Abstract, Blueprintable, EditInlineNew)
class INTOTHERADIUS_API UPRModificator : public UObject {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    UPRCondition* Condition;
    
    UPRModificator();

};

