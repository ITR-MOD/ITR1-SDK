#pragma once
#include "CoreMinimal.h"
#include "SaveInfo.h"
#include "SaveSlotInfo.generated.h"

USTRUCT(Atomic, BlueprintType)
struct INTOTHERADIUS_API FSaveSlotInfo {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    int32 SlotNr;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<int32, FSaveInfo> Saves;
    
    FSaveSlotInfo();
};

