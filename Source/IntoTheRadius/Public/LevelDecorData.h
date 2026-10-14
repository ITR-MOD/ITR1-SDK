#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LevelDecorData.generated.h"

USTRUCT(Atomic, BlueprintType)
struct INTOTHERADIUS_API FLevelDecorData {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<FString, FTransform> ItemTransforms;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FString> DestroyedItems;
    
    FLevelDecorData();
};

