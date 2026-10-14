#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "OpenedStashIDs.generated.h"

UCLASS(Blueprintable)
class INTOTHERADIUS_API UOpenedStashIDs : public UObject {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FString> OpenedIDs;
    
    UOpenedStashIDs();

};

