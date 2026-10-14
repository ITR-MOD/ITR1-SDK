#pragma once
#include "CoreMinimal.h"
#include "StaticMeshDataForInstanced.h"
#include "StaticMeshDataForInstancedCollection.generated.h"

USTRUCT(BlueprintType)
struct INTOTHERADIUS_API FStaticMeshDataForInstancedCollection {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FStaticMeshDataForInstanced> StaticMeshDataForInstanced;
    
    FStaticMeshDataForInstancedCollection();
};

