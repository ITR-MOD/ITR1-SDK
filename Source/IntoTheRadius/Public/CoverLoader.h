#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CoverSettings.h"
#include "PRBasicActor.h"
#include "Templates/SubclassOf.h"
#include "CoverLoader.generated.h"

class AActor;
class UCoverDebugComponent;

UCLASS(Blueprintable)
class INTOTHERADIUS_API ACoverLoader : public APRBasicActor {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FCoverSettings CoverSettings;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TSubclassOf<AActor> ShippingDebugActor;
    
private:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    UCoverDebugComponent* DebugComponent;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FVector> CoverPoints;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FVector> ShootingPositions;
    
public:
    ACoverLoader(const FObjectInitializer& ObjectInitializer);

};

