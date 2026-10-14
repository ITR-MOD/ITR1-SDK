#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SaveMaps.generated.h"

class UTextureRenderTarget2D;

USTRUCT(Atomic, BlueprintType)
struct INTOTHERADIUS_API FSaveMaps {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    int32 SlotNr;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    int32 SaveNr;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<FGameplayTag, UTextureRenderTarget2D*> RenderTargets;
    
    FSaveMaps();
};

