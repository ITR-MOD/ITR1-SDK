#pragma once
#include "CoreMinimal.h"
#include "PRBasicSceneComponent.h"
#include "PRClusterPointArtifact.generated.h"

class UBillboardComponent;

UCLASS(Blueprintable, ClassGroup=Custom, meta=(BlueprintSpawnableComponent))
class INTOTHERADIUS_API UPRClusterPointArtifact : public UPRBasicSceneComponent {
    GENERATED_BODY()
public:
private:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    UBillboardComponent* Billboard;
    
public:
    UPRClusterPointArtifact(const FObjectInitializer& ObjectInitializer);

};

