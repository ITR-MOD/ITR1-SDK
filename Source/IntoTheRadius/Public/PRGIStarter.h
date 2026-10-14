#pragma once
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameStarterInterface.h"
#include "PRSplashDesc.h"
#include "ReadyMarkInterface.h"
#include "PRGIStarter.generated.h"

UCLASS(Abstract, Blueprintable, NonTransient)
class INTOTHERADIUS_API UPRGIStarter : public UGameInstance, public IGameStarterInterface, public IReadyMarkInterface {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FPRSplashDesc> SplashDescriptions;
    
    UPRGIStarter();


    // Fix for true pure virtual functions not being implemented
};

