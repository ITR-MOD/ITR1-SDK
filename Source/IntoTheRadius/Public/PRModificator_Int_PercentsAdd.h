#pragma once
#include "CoreMinimal.h"
#include "PRModificator_Int.h"
#include "PRModificator_Int_PercentsAdd.generated.h"

UCLASS(Blueprintable, EditInlineNew)
class INTOTHERADIUS_API UPRModificator_Int_PercentsAdd : public UPRModificator_Int {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float PercentsAdd;
    
    UPRModificator_Int_PercentsAdd();

};

