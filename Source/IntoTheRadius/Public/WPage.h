#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ComputerPage.h"
#include "WPage.generated.h"

class UTextBlock;

UCLASS(Abstract, Blueprintable, EditInlineNew)
class INTOTHERADIUS_API UWPage : public UUserWidget, public IComputerPage {
    GENERATED_BODY()
public:
    UWPage();

protected:
    UFUNCTION(BlueprintCallable)
    void UpdateFooterTexts(UTextBlock* textPlayerLevel, UTextBlock* textMoney, UTextBlock* textTime);
    

    // Fix for true pure virtual functions not being implemented
};

