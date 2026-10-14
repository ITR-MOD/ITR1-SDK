#pragma once
#include "CoreMinimal.h"
#include "EPRSubtitleType.h"
#include "DialogueLine.generated.h"

USTRUCT(Atomic, BlueprintType)
struct INTOTHERADIUS_API FDialogueLine {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    EPRSubtitleType Type;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FText Text;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float TimeNoAudio;
    
    FDialogueLine();
};

