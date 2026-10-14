#pragma once
#include "CoreMinimal.h"
#include "PRModificator_Int.h"
#include "PRModificator_Int_Multiply.generated.h"

UCLASS(Blueprintable, EditInlineNew)
class INTOTHERADIUS_API UPRModificator_Int_Multiply : public UPRModificator_Int {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    int32 MultiplyBy;
    
    UPRModificator_Int_Multiply();

};

