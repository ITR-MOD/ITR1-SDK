#pragma once
#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "CoverDebugComponent.generated.h"

UCLASS(Blueprintable, ClassGroup=Custom, meta=(BlueprintSpawnableComponent))
class INTOTHERADIUS_API UCoverDebugComponent : public USceneComponent {
    GENERATED_BODY()
public:
private:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool bDebugDrawCoverPoints;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool bDebugDrawShootingPositions;
    
public:
    UCoverDebugComponent(const FObjectInitializer& ObjectInitializer);

};

