#pragma once
#include "CoreMinimal.h"
#include "IngameMenuControllerInterface.h"
#include "PRBasicActor.h"
#include "Templates/SubclassOf.h"
#include "IngameMenuController.generated.h"

class AActor;

UCLASS(Abstract, Blueprintable)
class INTOTHERADIUS_API AIngameMenuController : public APRBasicActor, public IIngameMenuControllerInterface {
    GENERATED_BODY()
public:
protected:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TSubclassOf<AActor> ingameMenuClass;
    
private:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    AActor* _ingameMenu;
    
public:
    AIngameMenuController(const FObjectInitializer& ObjectInitializer);

protected:
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnIngameMenuDeactivated();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnIngameMenuActivated();
    

    // Fix for true pure virtual functions not being implemented
};

