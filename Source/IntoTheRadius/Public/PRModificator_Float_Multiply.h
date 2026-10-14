#pragma once
#include "CoreMinimal.h"
#include "PRModificator_Float.h"
#include "PRModificator_Float_Multiply.generated.h"

UCLASS(Blueprintable, EditInlineNew)
class INTOTHERADIUS_API UPRModificator_Float_Multiply : public UPRModificator_Float {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float MultiplyBy;
    
    UPRModificator_Float_Multiply();

};

