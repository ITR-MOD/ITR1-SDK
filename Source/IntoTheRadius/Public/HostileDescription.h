#pragma once
#include "CoreMinimal.h"
#include "EJournalGroup.h"
#include "HostileDescription.generated.h"

class UTexture2D;

USTRUCT(Atomic, BlueprintType)
struct INTOTHERADIUS_API FHostileDescription {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    EJournalGroup Group;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FText Title;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    UTexture2D* Image;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FText JournalText;
    
    FHostileDescription();
};

