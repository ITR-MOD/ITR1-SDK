#pragma once
#include "CoreMinimal.h"
#include "PRModificator_Float.h"
#include "PRModificator_Float_Percentage.generated.h"

UCLASS(Blueprintable, EditInlineNew)
class INTOTHERADIUS_API UPRModificator_Float_Percentage : public UPRModificator_Float {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float Percentage;
    
    UPRModificator_Float_Percentage();

};

