#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "EDiscordReturnResult.h"
#include "OnDiscordResultDelegate.h"
#include "DiscordObject.generated.h"

class UDiscordObject;

UCLASS(Blueprintable)
class DISCORDSDK_API UDiscordObject : public UObject {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOnDiscordResult OnStateSet;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOnDiscordResult OnDetailsSet;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOnDiscordResult OnLargeImageSet;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOnDiscordResult OnSmallImageSet;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOnDiscordResult OnPartyIdSet;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOnDiscordResult OnPartySizeSet;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOnDiscordResult OnPartyMaxSet;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOnDiscordResult OnJoinSecretSet;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOnDiscordResult OnLargeTextSet;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOnDiscordResult OnSmallTextSet;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOnDiscordResult OnTimerStart;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOnDiscordResult OnTimerEnd;
    
    UDiscordObject();
    UFUNCTION(BlueprintCallable)
    void StopDiscordTimer();
    
    UFUNCTION(BlueprintCallable)
    void StartDiscordTimer();
    
    UFUNCTION(BlueprintCallable)
    void SetState(const FString& InNewState);
    
    UFUNCTION(BlueprintCallable)
    void SetSmallImageText(const FString& InKeyName);
    
    UFUNCTION(BlueprintCallable)
    void SetSmallImage(const FString& InKeyName);
    
    UFUNCTION(BlueprintCallable)
    void SetPartySize(const int32 InNewPartySize);
    
    UFUNCTION(BlueprintCallable)
    void SetPartyMax(const int32 InNewPartyMax);
    
    UFUNCTION(BlueprintCallable)
    void SetPartyId(const FString& InNewPartyId);
    
    UFUNCTION(BlueprintCallable)
    void SetLargeImageText(const FString& InKeyName);
    
    UFUNCTION(BlueprintCallable)
    void SetLargeImage(const FString& InKeyName);
    
    UFUNCTION(BlueprintCallable)
    void SetJoinSecret(const FString& InNewJoinSecret);
    
    UFUNCTION(BlueprintCallable)
    void SetDetails(const FString& InNewDetails);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FString GetDiscordResultString(EDiscordReturnResult InDiscordResult);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static UDiscordObject* GetDiscordObject();
    
    UFUNCTION(BlueprintCallable)
    static void DestroyDiscordObject();
    
    UFUNCTION(BlueprintCallable)
    static void CreateDiscordObject(const FString& InClientID, const bool bRequireDiscordRunning, const bool bStartElapsedTimer);
    
};

