#pragma once
#include "CoreMinimal.h"
#include "WPage.h"
#include "WMCPage_ShowcaseCategories.generated.h"

class UScrollBox;
class UWElement_ShowcaseCategory;

UCLASS(Blueprintable, EditInlineNew)
class INTOTHERADIUS_API UWMCPage_ShowcaseCategories : public UWPage {
    GENERATED_BODY()
//public:
 //   UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
 //   UScrollBox* SB_Categories;
    
protected:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    TArray<UWElement_ShowcaseCategory*> ShowcaseCategoriesArray;
    
public:
    UWMCPage_ShowcaseCategories();

protected:
    UFUNCTION(BlueprintCallable)
    void UpdateSelection(int32 selectedIndex);
    
public:
    UFUNCTION(BlueprintCallable)
    void CheckForHasNewItems();
    
};

