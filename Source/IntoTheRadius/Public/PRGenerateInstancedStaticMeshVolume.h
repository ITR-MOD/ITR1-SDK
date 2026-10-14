#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PhysicsVolume.h"
#include "StaticMeshDataForInstanced.h"
#include "StaticMeshDataForInstancedCollection.h"
#include "PRGenerateInstancedStaticMeshVolume.generated.h"

class AActor;
class UStaticMesh;
class UStaticMeshComponent;

UCLASS(Blueprintable)
class INTOTHERADIUS_API APRGenerateInstancedStaticMeshVolume : public APhysicsVolume {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TSoftObjectPtr<UStaticMesh> StaticMeshToSearch;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FStaticMeshDataForInstanced StaticMeshDataForCreatingInstanced;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool bClearVarsAfterEvent;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FString> ExcludedStaticMeshList;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool bUseMeshDefaultMaterial;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool bHideDuplicated;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool bCreateHism;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    uint8 MinSmCountToCreateInstanced;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TSoftClassPtr<AActor> TemplateInstancedActor;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<TSoftObjectPtr<UStaticMesh>, FStaticMeshDataForInstancedCollection> GatheredStaticMeshDataMap;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Export, meta=(AllowPrivateAccess=true))
    TArray<TSoftObjectPtr<UStaticMeshComponent>> StaticMeshComponentsHidden;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<TSoftObjectPtr<AActor>> InstancedStaticMeshActorsGenerated;
    
    APRGenerateInstancedStaticMeshVolume(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable)
    void ShowHiddenStaticMeshComponents();
    
    UFUNCTION(BlueprintCallable)
    void ShowAllGeneratedInstanced();
    
    UFUNCTION(BlueprintCallable)
    void RestoreHidden();
    
    UFUNCTION(BlueprintCallable)
    void RehideHiddenStaticMeshComponents();
    
    UFUNCTION(BlueprintCallable)
    void HideAllGeneratedInstanced();
    
    UFUNCTION(BlueprintCallable)
    void GatherMulitipleSmActors();
    
    UFUNCTION(BlueprintCallable)
    void DestroyAllGeneratedInstanced();
    
    UFUNCTION(BlueprintCallable)
    void CreateMultipleInstanced();
    
    UFUNCTION(BlueprintCallable)
    void CreateInstanced();
    
};

