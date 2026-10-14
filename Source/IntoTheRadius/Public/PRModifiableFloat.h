#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "PRModifiableFloat.generated.h"

class UPRModificator_Float;

UCLASS(Blueprintable, EditInlineNew)
class INTOTHERADIUS_API UPRModifiableFloat : public UObject {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float Value;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    TArray<UPRModificator_Float*> Modificators;
    
    UPRModifiableFloat();

};

