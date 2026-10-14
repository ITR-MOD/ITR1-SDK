#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Engine/LatentActionManager.h"
#include "GameplayTagContainer.h"
#include "EMissionState.h"
#include "IClearable.h"
#include "IPreparable.h"
#include "MissionsManager.generated.h"

class UMissionData;
class UMissionsManager;

UCLASS(Blueprintable)
class INTOTHERADIUS_API UMissionsManager : public UObject, public IIClearable, public IIPreparable {
    GENERATED_BODY()
public:
    UMissionsManager();

    UFUNCTION(BlueprintCallable, meta=(Latent, LatentInfo="LatentInfo", WorldContext="WorldContextObject"))
    void StartMissionFromIdLatent(bool& bSuccess, FGameplayTag GameplayTag, UObject* WorldContextObject, FLatentActionInfo LatentInfo);
    
    UFUNCTION(BlueprintCallable)
    void StartMission(const FString& MissionInstanceId, bool& bSuccess, UMissionData*& MissionData);
    
    UFUNCTION(BlueprintCallable)
    void SetSelectedMissionComplete(const FString& MissionInstanceId);
    
    UFUNCTION(BlueprintCallable)
    void ResetTutorialMissions();
    
private:
    UFUNCTION(BlueprintCallable)
    void OnPlayerLevelChanged(const int32 PlayerLevel);
    
public:
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void IsMissionSelected(const FString& MissionInstanceId, bool& bMissionSelected);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsMaxExecutingReached();
    
    UFUNCTION(BlueprintCallable)
    bool GetStoryMissionUnlockInfo(int32& FinishedAfterPrevStory, int32& RequiredAfterPrevStory);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetSelectedMissionsInState(const EMissionState State, TArray<UMissionData*>& SelectedMissions);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    TArray<UMissionData*> GetSelectedMissionsInProgress();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetSelectedMissions(TArray<UMissionData*>& SelectedMissions);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static UMissionsManager* GetMissionsManager();
    
    UFUNCTION(BlueprintCallable)
    int32 GetMissionsFinishedAfter(const FGameplayTag ConfigID, const bool bOnlySuccessful);
    
    UFUNCTION(BlueprintCallable)
    void GetMission(const FString& MissionInstanceId, UMissionData*& MissionData);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetFinishedMissions(TArray<UMissionData*>& FinishedMissions);
    
    UFUNCTION(BlueprintCallable)
    bool GetAvailableStoryMission(UMissionData*& StoryMission);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetActiveMissions(TMap<FGameplayTag, UMissionData*>& ActiveMissions);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetActiveMissionOfType(const FGameplayTag& Category, UMissionData*& MissionData);
    
    UFUNCTION(BlueprintCallable)
    bool GenerateNewMissions(bool bTide);
    
    UFUNCTION(BlueprintCallable)
    bool FinishMission(const FString& MissionInstanceId);
    
    UFUNCTION(BlueprintCallable)
    void CheckActive();
    
    UFUNCTION(BlueprintCallable)
    void AbortSelectedMission(const FString& MissionInstanceId);
    

    // Fix for true pure virtual functions not being implemented
};

