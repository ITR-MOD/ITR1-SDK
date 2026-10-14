#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "IClearable.h"
#include "IPreparable.h"
#include "SaveInfo.h"
#include "SaveSlotInfo.h"
#include "DataSaveManager.generated.h"

class UDataSaveManager;
class UInventoryItem;
class UPRGameData;

UCLASS(Blueprintable)
class INTOTHERADIUS_API UDataSaveManager : public UObject, public IIPreparable, public IIClearable {
    GENERATED_BODY()
public:
    UDataSaveManager();

    UFUNCTION(BlueprintCallable)
    void SetUseAlternateSaveLocation(bool Value);
    
    UFUNCTION(BlueprintCallable)
    static bool SaveStringToFileEditor(const FString& String, const FString& FilePath, const bool bAppend);
    
    UFUNCTION(BlueprintCallable)
    static bool SaveStringToFile(const FString& String, const FString& FilePath, const bool bAppend);
    
    UFUNCTION(BlueprintCallable)
    void SaveGameData(UPRGameData* Data, int32 SlotNr, int32 SaveNr);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsSlotWithIronman(const int32 Slot);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsSlotEmpty(const FSaveSlotInfo& SlotInfo);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool HasSaves();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool GetUseAlternateSaveLocation() const;
    
    UFUNCTION(BlueprintCallable)
    bool GetSlotInfo(const int32 Slot, FSaveSlotInfo& SlotInfo);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FString GetSavesPath();
    
    UFUNCTION(BlueprintCallable)
    bool GetSaveInfo(const int32 Slot, const int32 Save, FSaveInfo& SaveInfo);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FSaveInfo GetLastSaveInfo(FSaveSlotInfo SlotInfo);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static UDataSaveManager* GetDataSaveManager();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FString GetDataPath();
    
    UFUNCTION(BlueprintCallable)
    static bool FixInventoryItemPosition(UInventoryItem* Item);
    
    UFUNCTION(BlueprintCallable)
    void CopyScopedToAlternateSave();
    
    UFUNCTION(BlueprintCallable)
    void CopyAlternateToScopedSave();
    
    UFUNCTION(BlueprintCallable)
    void AutosaveGameData(const FString& Message);
    

    // Fix for true pure virtual functions not being implemented
};

