#pragma once
#include "CoreMinimal.h"
#include "MinimapMeshMappingConfig.generated.h"

class UStaticMesh;

USTRUCT(Atomic, BlueprintType)
struct INTOTHERADIUS_API FMinimapMeshMappingConfig {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<UStaticMesh*> appliedTo;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float Scale;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool rotateToMesh;
    
    FMinimapMeshMappingConfig();
};

