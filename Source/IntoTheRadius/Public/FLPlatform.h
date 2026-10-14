#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EPRControllerType.h"
#include "EPRHand.h"
#include "EPRHmdDeviceType.h"
#include "EPRPlatformType.h"
#include "FingerCurls.h"
#include "FLPlatform.generated.h"

UCLASS(Blueprintable)
class INTOTHERADIUS_API UFLPlatform : public UBlueprintFunctionLibrary {
    GENERATED_BODY()
public:
    UFLPlatform();

    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsVRControllerDefault();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsPlatformQuest3();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsPlatformAndroid();
    
    UFUNCTION(BlueprintCallable)
    static bool IsNativeOculus();
    
    UFUNCTION(BlueprintCallable)
    static bool HasVRFocus();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static EPRControllerType GetVRControllerType();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FString GetPlatformTypeString();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static EPRPlatformType GetPlatformType();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static void GetPixelDensityScale(float& ScaleX, float& ScaleY);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FIntPoint GetIdealRenderTargetSize();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static EPRHmdDeviceType GetHMDDeviceType();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FString GetGPUBrandName();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static void GetFingerCurls(const EPRHand hand, FFingerCurls& FingerCurls);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static EPRControllerType GetDefaultControllerType(EPRHmdDeviceType HMDDevice);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static void GetCurlsAndSplaysState(bool& LeftHandState, bool& RightHandState);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FString GetCPUVendorName();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FString GetCPUBrandName();
    
};

