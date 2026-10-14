#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IPooledActor.h"
#include "RadiusFirearmShellInfo.h"
#include "RadiusFirearmShellActor.generated.h"

class UPoolDisposeComponent;

UCLASS(Blueprintable)
class INTOTHERADIUS_API ARadiusFirearmShellActor : public AActor, public IIPooledActor {
    GENERATED_BODY()
public:
protected:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    UPoolDisposeComponent* PoolDisposeComponent;
    
public:
    ARadiusFirearmShellActor(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void Setup(const FRadiusFirearmShellInfo& Info);
    

    // Fix for true pure virtual functions not being implemented
};

