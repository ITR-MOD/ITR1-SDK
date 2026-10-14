#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ActorSpawnInfo.h"
#include "BaseItemSpawnInfo.generated.h"

USTRUCT(BlueprintType)
struct INTOTHERADIUS_API FBaseItemSpawnInfo : public FActorSpawnInfo {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool Preconfigured;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FString CustomUID;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool NotUnique;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FGameplayTag> CustomAdditionalTags;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float CustomDurabilityRatio;
    
    FBaseItemSpawnInfo();
};

