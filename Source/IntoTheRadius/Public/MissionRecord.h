#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MissionRecord.generated.h"

USTRUCT(Atomic, BlueprintType)
struct INTOTHERADIUS_API FMissionRecord {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FString ID;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FGameplayTag Tag;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FGameplayTag Weapon;
    
    FMissionRecord();
};

