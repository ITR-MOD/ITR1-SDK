#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "ConditionalPaperNote.h"
#include "Dialogue.h"
#include "DialoguesGroup.h"
#include "HostileDescription.h"
#include "IAsset.h"
#include "IPreparable.h"
#include "PaperNote.h"
#include "Record.h"
#include "RecordSubtitle.h"
#include "StoryConfig.generated.h"

class UPRItemConfig;
class USoundBase;
class UStoryConfig;

UCLASS(Blueprintable)
class INTOTHERADIUS_API UStoryConfig : public UDataAsset, public IIAsset, public IIPreparable {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FDialoguesGroup> CassetteGroups;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FDialogue EmptyCassette;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FPaperNote> PaperNotes;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FConditionalPaperNote> ConditionalPaperNotes;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<FGameplayTag, FHostileDescription> Monsters;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<FGameplayTag, FHostileDescription> anomalies;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<FGameplayTag, FHostileDescription> Other;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<FGameplayTag, FRecord> MissionTips;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<FGameplayTag, FDialogue> KatyaMonologues;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<FGameplayTag, FDialogue> FinalDialogs;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(AllowPrivateAccess=true))
    UPRItemConfig* FinalWeaponConfig;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FGameplayTag ChoiceKillSelfTag;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FGameplayTag ChoiceKillKatyaTag;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FGameplayTag ChoiceKillKatyaMonologueTag;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FGameplayTag ChoiceKillPresenceTag;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FGameplayTag ChoiceKillPresenceMonologueTag;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FGameplayTag> ItemTypeIDsToDestroyAfterFinale;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<FGameplayTag, FDialogue> Epilogues;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FText> HintWeaponOnSafety;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FText> HintWeaponNotLoaded;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FText> HintWeaponCockTheBolt;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FText> HintWeaponJammed;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FText> HintMagEmpty;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<USoundBase*> HintNoMissionTurretSound;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TArray<FText> HintNoMissionTurretSubtitle;
    
    UStoryConfig();

    UFUNCTION(BlueprintCallable)
    TArray<FGameplayTag> ParseRecordsTags(TArray<FGameplayTag> ItemAdditionalTags);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsJournalNote(const FGameplayTag& NoteID) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool HasNote(FGameplayTag NoteID);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool HasMissionTip(FGameplayTag TipID);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FRecordSubtitle GetSubtitleForTime(FRecord Record, float TimeSeconds);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static UStoryConfig* GetStoryConfig();
    
    UFUNCTION(BlueprintCallable)
    void GetRandomCassette(bool bMissingOnly, bool& bSuccess, FGameplayTag& CassetteTag, FDialogue& CassetteDialogue);
    
    UFUNCTION(BlueprintCallable)
    void GetPaperNote(FGameplayTag NoteID, bool& bSuccess, FPaperNote& Note);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FGameplayTag GetNPCJournalNote(const FGameplayTag& NpcID) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetMissionTip(FGameplayTag TipID, bool& bSuccess, FRecord& Record);
    
    UFUNCTION(BlueprintCallable)
    void GetKatyaMonologue(FGameplayTag Tag, bool& bSuccess, FDialogue& Monologue);
    
    UFUNCTION(BlueprintCallable)
    void GetCassette(FGameplayTag CassetteTag, bool& bSuccess, FDialogue& CassetteDialogue);
    

    // Fix for true pure virtual functions not being implemented
};

