#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Component_And_Transform.generated.h"

class UPrimitiveComponent;

USTRUCT(BlueprintType)
struct INTOTHERADIUS_API FComponent_And_Transform {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FTransform Transform;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    UPrimitiveComponent* PrimitiveComponent;
    
    FComponent_And_Transform();
};

