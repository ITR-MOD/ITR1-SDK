#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "PRNPCInfo.h"
#include "INPC.generated.h"

class AActor;
class UPrimitiveComponent;

UINTERFACE(Blueprintable)
class UINPC : public UInterface {
    GENERATED_BODY()
};

class IINPC : public IInterface {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void SetSnipingMode(const bool bSet);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void SetDebug(const bool NewDebug);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool IsCriticalHit(const UPrimitiveComponent* HitComponent, const FName BoneName) const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool IsAttacksLocked() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool IsArmored() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    FPRNPCInfo GetNPCInfo() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void GetIDs(FGameplayTag& TypeID, FString& InstanceId);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void Alert(AActor* Target, FTransform Location);
    
};

