#pragma once
#include "CoreMinimal.h"
#include "ActorsArray.generated.h"

class AActor;
class UObject;

USTRUCT(Atomic, BlueprintType)
struct INTOTHERADIUS_API FActorsArray {
    GENERATED_BODY()
public:
private:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<AActor*> Actors;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<bool> InUseFlags;
    
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    UObject* WorldContext;
    
    FActorsArray();
};

