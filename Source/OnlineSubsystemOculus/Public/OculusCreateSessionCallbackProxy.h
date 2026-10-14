#pragma once
#include "CoreMinimal.h"
#include "Net/OnlineBlueprintCallProxyBase.h"
#include "Net/OnlineBlueprintCallProxyBase.h"
#include "OculusCreateSessionCallbackProxy.generated.h"

class UOculusCreateSessionCallbackProxy;

UCLASS(Blueprintable, MinimalAPI)
class UOculusCreateSessionCallbackProxy : public UOnlineBlueprintCallProxyBase {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FEmptyOnlineDelegate OnSuccess;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FEmptyOnlineDelegate OnFailure;
    
    UOculusCreateSessionCallbackProxy();
    UFUNCTION(BlueprintCallable)
    static UOculusCreateSessionCallbackProxy* CreateSession(int32 PublicConnections, const FString& OculusMatchmakingPool);
    
};

