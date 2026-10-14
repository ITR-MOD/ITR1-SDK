#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "EMissionState.h"
#include "IClearable.h"
#include "TagToTagsList.h"
#include "MissionData.generated.h"

class AActor;
class UMissionAbstractObjectiveData;

UCLASS(Blueprintable)
class INTOTHERADIUS_API UMissionData : public UObject, public IIClearable {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FString InstanceId;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FGameplayTag ConfigID;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool Abortable;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    EMissionState State;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<UMissionAbstractObjectiveData*> Objectives;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<FGameplayTag, FGameplayTag> AdditionalCorpsePointsToLoot;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FTagToTagsList> AdditionalPointsToNPCs;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float TimeCreated;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float TimeStarted;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float TimeFinished;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    int32 TutorialStep;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<FString, FString> RawData;
    
    UMissionData();

    UFUNCTION(BlueprintCallable)
    void SetTutorStep(FGameplayTag SequencerID, int32 step);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool HaveDelivery();
    
    UFUNCTION(BlueprintCallable)
    void GetTotalReward(const bool FinishedOnly, int32& MoneyReward, int32& PlayerLevel);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    float GetTimeTillAbortAvailable();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FString GetStashUID(UMissionAbstractObjectiveData* Objective, FGameplayTag Tag) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    TArray<FGameplayTag> GetMissionPoints();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FText GetMissionName() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FText GetMissionDescription() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FGameplayTag GetMissionCategoryID() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FGameplayTag GetMainObjectivePoint() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FGameplayTag GetMainObjectiveLevel() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FString GetItemUID(UMissionAbstractObjectiveData* Objective, int32 Index) const;
    
    UFUNCTION(BlueprintCallable)
    void GetDuration(bool& bHaveDuration, FString& FinishDateString);
    
    UFUNCTION(BlueprintCallable)
    bool FinishObjective(UMissionAbstractObjectiveData* Objective, bool bIgnoreTutorialDelivery);
    
    UFUNCTION(BlueprintCallable)
    bool FinishAllObjectives(bool bIgnoreTutorialDelivery);
    
    UFUNCTION(BlueprintCallable)
    bool CheckPrimaryObjectivesCompleted();
    
    UFUNCTION(BlueprintCallable)
    void CheckDelivery(TArray<AActor*> ItemActors, bool bFinishObjectives, bool& bSuccess, TArray<AActor*>& ExcessiveActors);
    
    UFUNCTION(BlueprintCallable)
    void CheckCompletion(bool& bCompleted);
    

    // Fix for true pure virtual functions not being implemented
};

