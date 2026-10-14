#pragma once
#include "CoreMinimal.h"
#include "PRModificator_Float.h"
#include "PRModificator_Float_Add.generated.h"

UCLASS(Blueprintable, EditInlineNew)
class INTOTHERADIUS_API UPRModificator_Float_Add : public UPRModificator_Float {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float Add;
    
    UPRModificator_Float_Add();

};

