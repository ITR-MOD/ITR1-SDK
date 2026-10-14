#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Volume.h"
#include "ActorActivationVolume.generated.h"

class AActorActivationVolume;

UCLASS(Blueprintable)
class INTOTHERADIUS_API AActorActivationVolume : public AVolume {
    GENERATED_BODY()
public:
protected:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<AActorActivationVolume*> AdjacentVolumes;
    
public:
    AActorActivationVolume(const FObjectInitializer& ObjectInitializer);

};

