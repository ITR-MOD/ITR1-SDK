// Fill out your copyright notice in the Description page of Project Settings.


#include "ChuckBlueprintFunctionLibrary.h"
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/Object.h"
#include "UObject/UnrealType.h"
#include "GameplayTagsManager.h"
FName TagName;
FGameplayTag Tag;

//static FGameplayTag GetTagFromString(FName Tag)
//{
//    return UGameplayTagsManager::Get().RequestGameplayTag(FName(Tag));
//}

FGameplayTag UGetTagFromString::GetTagFromString(FName TagName, FGameplayTag Tag)
{
    return UGameplayTagsManager::Get().RequestGameplayTag(FName(TagName));
}
