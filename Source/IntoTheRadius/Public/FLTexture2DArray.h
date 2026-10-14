#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FLTexture2DArray.generated.h"

class UTexture2D;
class UTexture2DArray;
class UTexture2DArrayWrapper;

UCLASS(Blueprintable)
class INTOTHERADIUS_API UFLTexture2DArray : public UBlueprintFunctionLibrary {
    GENERATED_BODY()
public:
    UFLTexture2DArray();

private:
    UFUNCTION(BlueprintCallable)
    static TArray<UTexture2DArrayWrapper*> Texture2DArray_SplitTexturesIntoValidGroups(TArray<UTexture2D*> Textures);
    
    UFUNCTION(BlueprintCallable)
    static void Texture2DArray_SetTextures(UTexture2DArray* textureArray, TArray<UTexture2D*> Textures);
    
    UFUNCTION(BlueprintCallable)
    static TArray<UTexture2D*> Texture2DArray_GetTextures(UTexture2DArray* textureArray);
    
};

