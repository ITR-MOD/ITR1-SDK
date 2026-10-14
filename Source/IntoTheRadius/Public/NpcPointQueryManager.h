#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ENpcPointQueryType.h"
#include "IClearable.h"
#include "NpcPointQueryHandle.h"
#include "NpcPointQueryParam.h"
#include "PointQueryCompleteDelegateDelegate.h"
#include "NpcPointQueryManager.generated.h"

class UNpcPointQueryManager;

UCLASS(Blueprintable)
class INTOTHERADIUS_API UNpcPointQueryManager : public UObject, public IIClearable {
    GENERATED_BODY()
public:
    UNpcPointQueryManager();

    UFUNCTION(BlueprintCallable, BlueprintPure)
    static UNpcPointQueryManager* GetNpcPointQueryManager();
    
    UFUNCTION(BlueprintCallable)
    FNpcPointQueryHandle FindShootingPoint(const FNpcPointQueryParam& Param, FPointQueryCompleteDelegate FinishCallback, ENpcPointQueryType QueryType);
    
    UFUNCTION(BlueprintCallable)
    FNpcPointQueryHandle FindCoverPoint(const FNpcPointQueryParam& Param, FPointQueryCompleteDelegate FinishCallback);
    
    UFUNCTION(BlueprintCallable)
    void CancelQuery(UPARAM(Ref) FNpcPointQueryHandle& Handle);
    

    // Fix for true pure virtual functions not being implemented
};

