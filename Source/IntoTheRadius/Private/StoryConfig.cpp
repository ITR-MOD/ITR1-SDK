#include "StoryConfig.h"

UStoryConfig::UStoryConfig() {
    this->FinalWeaponConfig = NULL;
}

TArray<FGameplayTag> UStoryConfig::ParseRecordsTags(TArray<FGameplayTag> ItemAdditionalTags) {
    return TArray<FGameplayTag>();
}

bool UStoryConfig::IsJournalNote(const FGameplayTag& NoteID) const {
    return false;
}

bool UStoryConfig::HasNote(FGameplayTag NoteID) {
    return false;
}

bool UStoryConfig::HasMissionTip(FGameplayTag TipID) {
    return false;
}

FRecordSubtitle UStoryConfig::GetSubtitleForTime(FRecord Record, float TimeSeconds) {
    return FRecordSubtitle{};
}

UStoryConfig* UStoryConfig::GetStoryConfig() {
    return NULL;
}

void UStoryConfig::GetRandomCassette(bool bMissingOnly, bool& bSuccess, FGameplayTag& CassetteTag, FDialogue& CassetteDialogue) {
}

void UStoryConfig::GetPaperNote(FGameplayTag NoteID, bool& bSuccess, FPaperNote& Note) {
}

FGameplayTag UStoryConfig::GetNPCJournalNote(const FGameplayTag& NpcID) const {
    return FGameplayTag{};
}

void UStoryConfig::GetMissionTip(FGameplayTag TipID, bool& bSuccess, FRecord& Record) {
}

void UStoryConfig::GetKatyaMonologue(FGameplayTag Tag, bool& bSuccess, FDialogue& Monologue) {
}

void UStoryConfig::GetCassette(FGameplayTag CassetteTag, bool& bSuccess, FDialogue& CassetteDialogue) {
}


