// Fill out your copyright notice in the Description page of Project Settings.



#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagsManager.h"
#include "UObject/Object.h"
#include "UObject/UnrealType.h"
#include "ChuckBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS(meta = (ScriptName = "ChuckScripts"))
class INTOTHERADIUS_API UGetTagFromString : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()


		UFUNCTION(BlueprintPure, Category = "Chuck", meta = (DisplayName = "ToTag (String)"))
		static FGameplayTag GetTagFromString(FName TagName, FGameplayTag Tag);
	FName TagName;
	FGameplayTag Tag;
};
