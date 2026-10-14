#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Texture2DArrayWrapper.generated.h"

class UTexture2D;

UCLASS(Blueprintable)
class INTOTHERADIUS_API UTexture2DArrayWrapper : public UObject {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FString Postfix;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<UTexture2D*> Textures;
    
    UTexture2DArrayWrapper();

};

