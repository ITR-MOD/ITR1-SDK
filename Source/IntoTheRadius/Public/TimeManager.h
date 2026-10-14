#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/Object.h"
#include "IClearable.h"
#include "NoParamsDelegateDelegate.h"
#include "TimeManager.generated.h"

class UTimeManager;

UCLASS(Blueprintable)
class INTOTHERADIUS_API UTimeManager : public UObject, public IIClearable {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FNoParamsDelegate OnNightStart;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FNoParamsDelegate OnNightEnd;
    
    UTimeManager();

    UFUNCTION(BlueprintCallable)
    static bool SetDateTime(const FDateTime NewDateTime);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsNight() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsDay() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static UTimeManager* GetTimeManager();
    
    UFUNCTION(BlueprintCallable)
    void GetTime(int32& Hour, int32& Minute);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    float GetSecondsTill(const float Hour);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static float GetSecondsPassed();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static float GetHoursPassed();
    
    UFUNCTION(BlueprintCallable)
    float GetHour();
    
    UFUNCTION(BlueprintCallable)
    void GetDateTimeStrings(FString& Date, FString& DayOfWeek, FString& Hour, FString& Minute, FString& Second);
    
    UFUNCTION(BlueprintCallable)
    static FDateTime GetDateTime();
    
    UFUNCTION(BlueprintCallable)
    void DebugSetHour(const float Hour);
    
    UFUNCTION(BlueprintCallable)
    void AddTimeTill(float Hour);
    
    UFUNCTION(BlueprintCallable)
    void AddSeconds(const float Seconds);
    
    UFUNCTION(BlueprintCallable)
    void AddHours(const float Hours);
    

    // Fix for true pure virtual functions not being implemented
};

