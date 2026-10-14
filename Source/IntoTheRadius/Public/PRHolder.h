#pragma once
#include "CoreMinimal.h"
#include "IItemHolder.h"
#include "PRBasicActor.h"
#include "PRHolder.generated.h"

class AActor;

UCLASS(Blueprintable)
class INTOTHERADIUS_API APRHolder : public APRBasicActor, public IIItemHolder {
    GENERATED_BODY()
public:
protected:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<AActor*> OverlappedActors;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool bDisabledInteraction;
    
public:
    APRHolder(const FObjectInitializer& ObjectInitializer);


    // Fix for true pure virtual functions not being implemented
};

