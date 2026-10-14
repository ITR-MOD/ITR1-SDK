#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EPRSubtitleType.h"
#include "SubtitlesControllerInterface.generated.h"

UINTERFACE(Blueprintable)
class USubtitlesControllerInterface : public UInterface {
    GENERATED_BODY()
};

class ISubtitlesControllerInterface : public IInterface {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void ShowSubtitlesString(const FString& Text, const float& Duration, const EPRSubtitleType& Type, const bool bSwitchable);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void ShowSubtitles(const FText& Text, const float& Duration, const EPRSubtitleType& Type, const bool bSwitchable);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void ShowSaveIcon(bool bAutoSave);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void SetDressingMode(bool Active);
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void HideSubtitles();
    
};

