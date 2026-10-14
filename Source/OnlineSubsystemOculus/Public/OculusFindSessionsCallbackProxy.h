#pragma once
#include "CoreMinimal.h"
#include "Net/OnlineBlueprintCallProxyBase.h"
#include "FindSessionsCallbackProxy.h"
#include "OculusFindSessionsCallbackProxy.generated.h"

class UOculusFindSessionsCallbackProxy;

UCLASS(Blueprintable, MinimalAPI)
class UOculusFindSessionsCallbackProxy : public UOnlineBlueprintCallProxyBase {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FBlueprintFindSessionsResultDelegate OnSuccess;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FBlueprintFindSessionsResultDelegate OnFailure;
    
    UOculusFindSessionsCallbackProxy();
    UFUNCTION(BlueprintCallable)
    static UOculusFindSessionsCallbackProxy* FindModeratedSessions(int32 MaxResults);
    
    UFUNCTION(BlueprintCallable)
    static UOculusFindSessionsCallbackProxy* FindMatchmakingSessions(int32 MaxResults, const FString& OculusMatchmakingPool);
    
};

