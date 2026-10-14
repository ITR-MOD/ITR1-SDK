#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "EMissionObjectiveImportance.h"
#include "MissionAbstractObjectiveData.generated.h"

UCLASS(Abstract, Blueprintable)
class INTOTHERADIUS_API UMissionAbstractObjectiveData : public UObject {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    EMissionObjectiveImportance Importance;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    int32 RewardMoney;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    int32 RewardPlayerLevel;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool Finished;
    
    UMissionAbstractObjectiveData();

    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool HaveDelivery();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    int32 GetRewardMoney() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FText GetObjectiveShortName();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FText GetObjectiveLongName();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    TArray<FGameplayTag> GetMissionPoints();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FString GetDeliveryCountString();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    int32 GetDeliveryCountObjectives();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FString GetCountString();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    int32 GetCountObjectives();
    
    UFUNCTION(BlueprintCallable)
    void CollectDeliveryTags(TArray<FGameplayTag>& DeliveryTags);
    
    UFUNCTION(BlueprintCallable)
    bool CheckObjectiveCompletion();
    
};

