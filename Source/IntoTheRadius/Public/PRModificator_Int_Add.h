#pragma once
#include "CoreMinimal.h"
#include "PRModificator_Int.h"
#include "PRModificator_Int_Add.generated.h"

UCLASS(Blueprintable, EditInlineNew)
class INTOTHERADIUS_API UPRModificator_Int_Add : public UPRModificator_Int {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    int32 Add;
    
    UPRModificator_Int_Add();

};

