#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/NoExportTypes.h"
#include "Templates/SubclassOf.h"
#include "CoverRuntimeDataHolder.generated.h"

class AActor;
class UCoverRuntimeDataHolder;

UCLASS(Blueprintable)
class INTOTHERADIUS_API UCoverRuntimeDataHolder : public UObject {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TSubclassOf<AActor> ShippingDebugActor;
    
    UCoverRuntimeDataHolder();

    UFUNCTION(BlueprintCallable)
    void ToggleDebug();
    
    UFUNCTION(BlueprintCallable)
    void OnLevelUnloaded();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool GetDebugEnabled() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static UCoverRuntimeDataHolder* GetCoverRuntimeDataHolder();
    
    UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
    void DrawShippingDebug(const UObject* WorldContextObject, const FVector CurrentLocation, float Radius);
    
    UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
    void DebugDraw(const UObject* WorldContextObject);
    
};

