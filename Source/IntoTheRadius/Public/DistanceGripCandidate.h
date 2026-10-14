#pragma once
#include "CoreMinimal.h"
#include "DistanceGripCandidate.generated.h"

class AGrippableStaticMeshActor;
class UPrimitiveComponent;

USTRUCT(BlueprintType)
struct FDistanceGripCandidate {
    GENERATED_BODY()
public:
private:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    AGrippableStaticMeshActor* Actor;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    UPrimitiveComponent* Component;
    
public:
    INTOTHERADIUS_API FDistanceGripCandidate();
};

