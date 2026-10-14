#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "WShopComputer.generated.h"

class AActor;
class UWMCPage_ShowcaseCategories;
class UWPage;

UCLASS(Blueprintable, EditInlineNew)
class INTOTHERADIUS_API UWShopComputer : public UUserWidget {
    GENERATED_BODY()
//public:
 //  UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
 //   UWPage* WMCPage_Empty;
    
   // UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
 //   UWPage* WMCPage_ShopBin;
    
 //   UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
 //   UWPage* WMCPage_ShopControls;
    
 //   UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
 //   UWMCPage_ShowcaseCategories* WMCPage_ShowcaseCategories;
    
 //   UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
 //   UWPage* WMCPage_TakeItems;
    
    UWShopComputer();

    UFUNCTION(BlueprintCallable)
    void UpdateNewItems();
    
    UFUNCTION(BlueprintCallable)
    void ShowTakeItems(bool bShow);
    
    UFUNCTION(BlueprintCallable)
    void ShowShopBin(bool bShow);
    
private:
    UFUNCTION(BlueprintCallable)
    void OnSCTick();
    
    UFUNCTION(BlueprintCallable)
    void OnLevelStarted(FGameplayTag LevelTag);
    
    UFUNCTION(BlueprintCallable)
    void OnGameStarted();
    
public:
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsShopBinVisible() const;
    
    UFUNCTION(BlueprintCallable)
    void InitializeByActor(AActor* Actor);
    
};

