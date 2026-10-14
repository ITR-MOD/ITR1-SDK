#pragma once
#include "CoreMinimal.h"
#include "PaperNote.h"
#include "ConditionalPaperNote.generated.h"

class UPRCondition;

USTRUCT(BlueprintType)
struct INTOTHERADIUS_API FConditionalPaperNote : public FPaperNote {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    TArray<UPRCondition*> Conditions;
    
    FConditionalPaperNote();
};

