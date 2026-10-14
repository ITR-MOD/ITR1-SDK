#pragma once
#include "CoreMinimal.h"
#include "PRModificator_Int.h"
#include "PRModificator_Int_Percentage.generated.h"

UCLASS(Blueprintable, EditInlineNew)
class INTOTHERADIUS_API UPRModificator_Int_Percentage : public UPRModificator_Int {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float Percentage;
    
    UPRModificator_Int_Percentage();

};

