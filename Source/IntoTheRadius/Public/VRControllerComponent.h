#pragma once
#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "VRControllerComponent.generated.h"

class UMaterialInstanceDynamic;

UCLASS(Blueprintable, ClassGroup=Custom, meta=(BlueprintSpawnableComponent))
class INTOTHERADIUS_API UVRControllerComponent : public USceneComponent {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool bLeft;
    
    UVRControllerComponent(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void UpdateControllerMesh(USceneComponent* Parent);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void SetMeshVisibility(bool Visible);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, BlueprintPure)
    void IsMeshVisibile(bool& Visible);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, BlueprintPure)
    void GetMaterial(UMaterialInstanceDynamic*& Material);
    
};

