#pragma once
#include "CoreMinimal.h"
#include "PRBasicActor.h"
#include "PRSplashDesc.h"
#include "PRTransitionHint.generated.h"

class USceneComponent;
class UWidgetComponent;

UCLASS(Abstract, Blueprintable)
class INTOTHERADIUS_API APRTransitionHint : public APRBasicActor {
    GENERATED_BODY()
public:
protected:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    USceneComponent* Root;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    USceneComponent* HintWidgetParentComponent;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    UWidgetComponent* HintWidgetComponent;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FPRSplashDesc> SplashDescriptions_Quest;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FPRSplashDesc SplashDescForHint_Quest;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FPRSplashDesc> SplashDescriptions_Pico;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FPRSplashDesc SplashDescForHint_Pico;
    
public:
    APRTransitionHint(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable)
    void ShowTransitionHint();
    
    UFUNCTION(BlueprintCallable)
    void HideTransitionHint();
    
};

