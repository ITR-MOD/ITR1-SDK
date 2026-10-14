#pragma once
#include "CoreMinimal.h"
#include "RadiusProjectile.h"
#include "RadiusShrapnelProjectile.generated.h"

class URadiusShrapnelComponent;

UCLASS(Blueprintable)
class INTOTHERADIUS_API ARadiusShrapnelProjectile : public ARadiusProjectile {
    GENERATED_BODY()
public:
protected:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    URadiusShrapnelComponent* ShrapnelComponent;
    
public:
    ARadiusShrapnelProjectile(const FObjectInitializer& ObjectInitializer);

};

