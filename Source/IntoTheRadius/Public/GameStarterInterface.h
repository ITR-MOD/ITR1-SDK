#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameStarterInterface.generated.h"

UINTERFACE(Blueprintable)
class UGameStarterInterface : public UInterface {
    GENERATED_BODY()
};

class IGameStarterInterface : public IInterface {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void ShowStartupLoadingSplash() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void GetPSOTasksNumbers(int32& NumRemaining, int32& NumTotal) const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool GameStarted() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool GameShuttingDown() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void CheckGameStart();
    
};

