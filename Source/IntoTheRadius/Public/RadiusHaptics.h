#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "EHapticsType.h"
#include "RadiusHaptics.generated.h"

USTRUCT(BlueprintType)
struct INTOTHERADIUS_API FRadiusHaptics : public FTableRowBase {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    EHapticsType Type;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FString> FeedbackId;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float Intensity;
    
    FRadiusHaptics();
};

