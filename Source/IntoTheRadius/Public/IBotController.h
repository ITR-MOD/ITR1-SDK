#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/NoExportTypes.h"
#include "ECharCommand.h"
#include "IBotController.generated.h"

class AActor;
class APRBasicActor;
class APawn;
class UPRNpcPerceptionComponent;

UINTERFACE(Blueprintable, MinimalAPI)
class UIBotController : public UInterface {
    GENERATED_BODY()
};

class IIBotController : public IInterface {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void SetTargetActor(AActor* Target);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void SetLeader(APRBasicActor* NewLeader);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void SetDesiredPosition(FVector Pos);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void ReportEnemyLocation(FVector Loc);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void RemoveSquadMember(APawn* Pawn);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OrderCover();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OrderCompleted(APawn* Pawn);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OrderAttack();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    FVector GetTargetLocation();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    AActor* GetTargetActor();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    TArray<AActor*> GetSquadMembers();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    UPRNpcPerceptionComponent* GetPerception();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    FVector GetDesiredPosition();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void GetCommand(ECharCommand& CurrentCommand);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void AddSquadMember(APawn* Pawn);
    
};

