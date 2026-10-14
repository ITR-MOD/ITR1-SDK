#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PRAnimInstance.generated.h"

class AActor;
class USkeletalMeshComponent;

UCLASS(Blueprintable, NonTransient)
class INTOTHERADIUS_API UPRAnimInstance : public UAnimInstance {
    GENERATED_BODY()
public:
private:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    USkeletalMeshComponent* _smc;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    AActor* _actor;
    
public:
    UPRAnimInstance();

protected:
    UFUNCTION(BlueprintCallable)
    USkeletalMeshComponent* GetSMC();
    
    UFUNCTION(BlueprintCallable)
    AActor* GetActor();
    
};

