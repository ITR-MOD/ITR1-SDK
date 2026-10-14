#pragma once
#include "CoreMinimal.h"
#include "EPRSubtitleType.h"
#include "MinMaxFloat.h"
#include "RecordSubtitle.generated.h"

USTRUCT(Atomic, BlueprintType)
struct INTOTHERADIUS_API FRecordSubtitle {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    EPRSubtitleType Type;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FText Text;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FMinMaxFloat TimeFromTo;
    
    FRecordSubtitle();
};

