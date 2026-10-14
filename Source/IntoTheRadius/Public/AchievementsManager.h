#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ERadiusAchievement.h"
#include "ERadiusLeaderboard.h"
#include "IClearable.h"
#include "AchievementsManager.generated.h"

class UAchievementsManager;
class UInventoryItem;

UCLASS(Blueprintable)
class INTOTHERADIUS_API UAchievementsManager : public UObject, public IIClearable {
    GENERATED_BODY()
public:
    UAchievementsManager();

    UFUNCTION(BlueprintCallable)
    void UpdateLeaderboardProgress(const ERadiusLeaderboard Leaderboard, const int32 Value);
    
    UFUNCTION(BlueprintCallable, Exec)
    void ResetAchievementProgress();
    
    UFUNCTION(BlueprintCallable)
    void ReportAchievement(const ERadiusAchievement Achievement, const int32 Value);
    
private:
    UFUNCTION(BlueprintCallable)
    void OnItemTaken(UInventoryItem* Item);
    
    UFUNCTION(BlueprintCallable)
    void OnItemBoughtEvent(UInventoryItem* InventoryItem);
    
public:
    UFUNCTION(BlueprintCallable, Exec)
    void LogCachedAchievements();
    
    UFUNCTION(BlueprintCallable)
    void Initialize();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static UAchievementsManager* GetAchievementsManager();
    
    UFUNCTION(BlueprintCallable)
    void CompleteAchievement(const ERadiusAchievement Achievement);
    

    // Fix for true pure virtual functions not being implemented
};

