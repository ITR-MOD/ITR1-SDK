#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WElement_ShowcaseCategory.generated.h"

class UImage;

UCLASS(Blueprintable, EditInlineNew)
class INTOTHERADIUS_API UWElement_ShowcaseCategory : public UUserWidget {
    GENERATED_BODY()
//public:
//protected:
//    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
//    UImage* Exclamation_Mark;
//    
//    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
//    UImage* Image_Selected;
    
public:
    UWElement_ShowcaseCategory();

    UFUNCTION(BlueprintCallable)
    void ShowNewItemExclamationMark(bool bShow);
    
};

