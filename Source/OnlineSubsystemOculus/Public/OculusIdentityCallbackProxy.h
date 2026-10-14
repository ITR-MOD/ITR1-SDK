#pragma once
#include "CoreMinimal.h"
#include "Net/OnlineBlueprintCallProxyBase.h"
#include "OculusIdentityFailureResultDelegate.h"
#include "OculusIdentitySuccessResultDelegate.h"
#include "OculusIdentityCallbackProxy.generated.h"

class UOculusIdentityCallbackProxy;

UCLASS(Blueprintable, MinimalAPI)
class UOculusIdentityCallbackProxy : public UOnlineBlueprintCallProxyBase {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOculusIdentitySuccessResult OnSuccess;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOculusIdentityFailureResult OnFailure;
    
    UOculusIdentityCallbackProxy();
    UFUNCTION(BlueprintCallable)
    static UOculusIdentityCallbackProxy* GetOculusIdentity(int32 LocalUserNum);
    
};

