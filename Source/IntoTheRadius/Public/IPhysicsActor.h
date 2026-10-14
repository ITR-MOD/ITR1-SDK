#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IPhysicsActor.generated.h"

class UPrimitiveComponent;

UINTERFACE(Blueprintable)
class UIPhysicsActor : public UInterface {
    GENERATED_BODY()
};

class IIPhysicsActor : public IInterface {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    UPrimitiveComponent* GetPhysicsRootComponent() const;
    
};

