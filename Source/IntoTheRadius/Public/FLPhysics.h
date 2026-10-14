#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FLPhysics.generated.h"

class AActor;
class UPrimitiveComponent;

UCLASS(Blueprintable)
class INTOTHERADIUS_API UFLPhysics : public UBlueprintFunctionLibrary {
    GENERATED_BODY()
public:
    UFLPhysics();

    UFUNCTION(BlueprintCallable)
    static UPrimitiveComponent* ToggleSimulatePhysics(const AActor* Actor, const bool bState);
    
};

