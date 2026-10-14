#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "PRModifiableInt.generated.h"

class UPRModificator_Int;

UCLASS(Blueprintable, EditInlineNew)
class INTOTHERADIUS_API UPRModifiableInt : public UObject {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    int32 Value;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    TArray<UPRModificator_Int*> Modificators;
    
    UPRModifiableInt();

};

