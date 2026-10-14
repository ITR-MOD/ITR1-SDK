#pragma once
#include "CoreMinimal.h"
#include "EJournalGroup.h"
#include "RecordSubtitle.h"
#include "Record.generated.h"

class USoundBase;

USTRUCT(Atomic, BlueprintType)
struct INTOTHERADIUS_API FRecord {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    EJournalGroup Group;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FText Name;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FText JournalText;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    USoundBase* Sound;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FRecordSubtitle> Subtitles;
    
    FRecord();
};

