#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/NoExportTypes.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "IAsset.h"
#include "Templates/SubclassOf.h"
#include "AmmoConfig.generated.h"

class AActor;
class UAmmoConfig;
class UDataTable;
class UStaticMesh;
class UStaticMeshComponent;

UCLASS(Blueprintable)
class INTOTHERADIUS_API UAmmoConfig : public UDataAsset, public IIAsset {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    UDataTable* CommonInfoTable;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    int32 MeshPrimitiveDataIndex;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float ProjectileForwardSpawnOffset;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TSubclassOf<AActor> BulletClassActor;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TSubclassOf<AActor> GrenadeClassActor;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TSubclassOf<AActor> ShrapnelClassActor;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FGameplayTag> ShrapnelTags;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TSubclassOf<AActor> ExplosionClassActor;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FVector2D AirExplosionShrapnelAccuracyRange;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FVector2D AirExplosionBulletAccuracyRange;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FVector ExplosionProjectilesOriginOffset;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FVector ExplosionProjectilesBoxExtend;
    
    UAmmoConfig();

    UFUNCTION(BlueprintCallable)
    static void SetupAmmoMeshComponent(const FGameplayTag& AmmoTag, UStaticMeshComponent* AmmoMeshComponent, const bool IsFired);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsTakenOutAmmo(AActor* Actor);
    
    UFUNCTION(BlueprintCallable)
    bool IsShrapnelAmmo(FGameplayTag AmmoTag);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsHeldAmmo(AActor* Actor);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsAmmo(const AActor* Actor);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static UStaticMesh* GetAmmoWholeMesh(const FGameplayTag& AmmoType);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FText GetAmmoTypeName(const FGameplayTag& AmmoType);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FLinearColor GetAmmoTypeColor(const FGameplayTag& AmmoType);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static UAmmoConfig* GetAmmoTagsConfig();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static UStaticMesh* GetAmmoShellMesh(const FGameplayTag& AmmoType);
    

    // Fix for true pure virtual functions not being implemented
};

