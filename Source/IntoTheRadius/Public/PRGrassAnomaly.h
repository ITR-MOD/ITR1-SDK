#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PRAnomaly.h"
#include "PlayerLightSensitiveInterface.h"
#include "PRGrassAnomaly.generated.h"

class UStaticMeshComponent;

UCLASS(Blueprintable)
class INTOTHERADIUS_API APRGrassAnomaly : public APRAnomaly, public IPlayerLightSensitiveInterface {
    GENERATED_BODY()
public:
protected:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float MinScaleValue;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float MaxScaleValue;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float ShrinkRatio;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float GrowthRatio;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float DamageHeight;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FVector ComponentParameterBase;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FVector ComponentParameterRatio;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    TArray<UStaticMeshComponent*> ComponentsArray;
    
public:
    APRGrassAnomaly(const FObjectInitializer& ObjectInitializer);

protected:
    UFUNCTION(BlueprintCallable)
    void ReduceComponent(const int32 Index, const float Value);
    
    UFUNCTION(BlueprintCallable)
    void Initialize();
    

    // Fix for true pure virtual functions not being implemented
};

