#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PRBasicActorComponent.h"
#include "TeleportPointInfo.h"
#include "Templates/SubclassOf.h"
#include "RadiusPlayerMovementComponent.generated.h"

class ACharacter;
class ARadiusTeleportArcController;
class UCapsuleComponent;
class UCharacterMovementComponent;
class UPlayerStatsComponent;

UCLASS(Blueprintable, ClassGroup=Custom, meta=(BlueprintSpawnableComponent))
class INTOTHERADIUS_API URadiusPlayerMovementComponent : public UPRBasicActorComponent {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TSubclassOf<ARadiusTeleportArcController> ArcControllerClass;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    ARadiusTeleportArcController* ArcController;
    
private:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    ACharacter* CharacterOwner;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    UCapsuleComponent* OwnerCapsule;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    UCharacterMovementComponent* MovementComponent;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    UPlayerStatsComponent* StatsComponent;
    
public:
    URadiusPlayerMovementComponent(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable)
    void StopMovement();
    
    UFUNCTION(BlueprintCallable)
    void ProcessMovementTeleport(const float DeltaSeconds);
    
    UFUNCTION(BlueprintCallable)
    void ProcessMovementNormal(const float DeltaSeconds);
    
    UFUNCTION(BlueprintCallable)
    void ProcessMovement(const float DeltaSeconds);
    
    UFUNCTION(BlueprintCallable)
    TArray<FTeleportPointInfo> PreparePointsToTeleport(const FVector& From, const FVector& To);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsTeleporting() const;
    
};

