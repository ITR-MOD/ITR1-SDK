#pragma once
#include "CoreMinimal.h"
#include "EventRecord.generated.h"

USTRUCT(BlueprintType)
struct FEventRecord {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FString Value;
    
    CMANALYTICS_API FEventRecord();
};

