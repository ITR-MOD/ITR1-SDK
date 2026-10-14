#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "PRBasicActor.h"
#include "CharBroadcast.generated.h"

class AActor;
class ACharBroadcast;
class UObject;
class USphereComponent;

UCLASS(Blueprintable)
class INTOTHERADIUS_API ACharBroadcast : public APRBasicActor {
    GENERATED_BODY()
public:
private:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    USphereComponent* Sphere;
    
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float Radius;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float LifeTime;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FGameplayTag MissionKillPoint;
    
    ACharBroadcast(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContext"))
    static ACharBroadcast* SpawnBroadcast(UObject* WorldContext, FTransform SpawnTransform, FGameplayTag MissionKillPointTag, float LifeSpan, float SphereRadius);
    
    UFUNCTION(BlueprintCallable)
    void AlertOthers(AActor* Caller, AActor*& Target, FTransform Location);
    
};

