#pragma once
#include "CoreMinimal.h"
#include "EventRecord.h"
#include "EventRecords.generated.h"

USTRUCT(BlueprintType)
struct FEventRecords {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FEventRecord> Records;
    
    CMANALYTICS_API FEventRecords();
};

