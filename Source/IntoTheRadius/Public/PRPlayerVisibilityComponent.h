#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "PRBasicActorComponent.h"
#include "PRPlayerVisibilityComponent.generated.h"

class APREnvironment;
class APRPlayerCharacter;
class UCurveFloat;
class UPrimitiveComponent;

UCLASS(Blueprintable, ClassGroup=Custom, meta=(BlueprintSpawnableComponent))
class INTOTHERADIUS_API UPRPlayerVisibilityComponent : public UPRBasicActorComponent {
    GENERATED_BODY()
public:
protected:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    APRPlayerCharacter* Player;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    TArray<UPrimitiveComponent*> PlayerPrimitiveList;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    APREnvironment* Environment;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    UCurveFloat* DistanceFactorCurve;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    UCurveFloat* SunFactorCurve;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    UCurveFloat* HeadlampFactorCurve;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    UCurveFloat* VelocityFactorCurve;
    
public:
    UPRPlayerVisibilityComponent(const FObjectInitializer& ObjectInitializer);

private:
    UFUNCTION(BlueprintCallable)
    void InitOnLevelStart(FGameplayTag LevelTag);
    
protected:
    UFUNCTION(BlueprintCallable)
    void InitOnGameStart();
    
private:
    UFUNCTION(BlueprintCallable)
    void CleanupOnTransitionStart();
    
};

