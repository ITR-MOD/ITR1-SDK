#pragma once
#include "CoreMinimal.h"
#include "Net/OnlineBlueprintCallProxyBase.h"
#include "OculusEntitlementCheckResultDelegate.h"
#include "OculusEntitlementCallbackProxy.generated.h"

class UOculusEntitlementCallbackProxy;

UCLASS(Blueprintable, MinimalAPI)
class UOculusEntitlementCallbackProxy : public UOnlineBlueprintCallProxyBase {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOculusEntitlementCheckResult OnSuccess;
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOculusEntitlementCheckResult OnFailure;
    
    UOculusEntitlementCallbackProxy();
    UFUNCTION(BlueprintCallable)
    static UOculusEntitlementCallbackProxy* VerifyEntitlement();
    
};

