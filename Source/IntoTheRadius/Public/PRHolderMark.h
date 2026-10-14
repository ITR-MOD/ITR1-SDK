#pragma once
#include "CoreMinimal.h"
#include "PRBasicActor.h"
#include "PRHolderMark.generated.h"

class AActor;
class UHolsterComponent;

UCLASS(Blueprintable)
class INTOTHERADIUS_API APRHolderMark : public APRBasicActor {
    GENERATED_BODY()
public:
protected:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    AActor* ActorInLeftHand;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    AActor* ActorInRightHand;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    UHolsterComponent* holsterRef;
    
public:
    APRHolderMark(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void Update();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void LinkActor(AActor* ActorInHand, bool bIsLeft);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void InitHolsterRef(UHolsterComponent* NewHolsterRef);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void Highlight(bool bIsLeft);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void GetDistances(bool& bLeftInRange, float& leftDistance, bool& bRightInRange, float& rightDistance);
    
    UFUNCTION(BlueprintCallable)
    AActor* GetActorInRightHand();
    
    UFUNCTION(BlueprintCallable)
    AActor* GetActorInLeftHand();
    
};

