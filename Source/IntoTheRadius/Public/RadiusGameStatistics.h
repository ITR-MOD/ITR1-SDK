#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "ERadiusCommonStatisticsType.h"
#include "IPreparable.h"
#include "RadiusGameStatistics.generated.h"

class URadiusGameStatistics;

UCLASS(Blueprintable)
class INTOTHERADIUS_API URadiusGameStatistics : public UObject, public IIPreparable {
    GENERATED_BODY()
public:
    URadiusGameStatistics();

    UFUNCTION(BlueprintCallable, BlueprintPure)
    FString GetStatisticsValueString(const ERadiusCommonStatisticsType Type) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FText GetStatisticsTitle(const ERadiusCommonStatisticsType Type) const;
    
private:
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static void GetRadiusGameStatistics(URadiusGameStatistics*& RadiusGameStatistics);
    
public:
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetPlayerKills(TMap<FGameplayTag, int32>& Enemies, TMap<FGameplayTag, int32>& Weapon) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetPlayerDeaths(TMap<FGameplayTag, int32>& Deaths) const;
    

    // Fix for true pure virtual functions not being implemented
};

