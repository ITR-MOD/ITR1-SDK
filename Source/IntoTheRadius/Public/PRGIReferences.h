#pragma once
#include "CoreMinimal.h"
#include "PRGIStarter.h"
#include "Templates/SubclassOf.h"
#include "PRGIReferences.generated.h"

class IInterface;
class UInterface;
class UObject;

UCLASS(Abstract, Blueprintable, NonTransient)
class INTOTHERADIUS_API UPRGIReferences : public UPRGIStarter {
    GENERATED_BODY()
public:
private:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<TSubclassOf<UInterface>, UObject*> Controllers;
    
public:
    UPRGIReferences();

    UFUNCTION(BlueprintCallable, BlueprintPure)
    static UObject* GetControllerReference(const TSubclassOf<UInterface> Interface);
    
};

