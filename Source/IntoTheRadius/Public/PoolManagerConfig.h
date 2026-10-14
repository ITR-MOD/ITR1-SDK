#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "IAsset.h"
#include "PoolActorConfig.h"
#include "PoolManagerConfig.generated.h"

class UPoolManagerConfig;

UCLASS(Blueprintable)
class INTOTHERADIUS_API UPoolManagerConfig : public UDataAsset, public IIAsset {
    GENERATED_BODY()
public:
private:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FPoolActorConfig> ActorConfigs;
    
public:
    UPoolManagerConfig();

    UFUNCTION(BlueprintCallable, BlueprintPure)
    static UPoolManagerConfig* GetPoolManagerConfig();
    

    // Fix for true pure virtual functions not being implemented
};

