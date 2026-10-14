#pragma once

#include "CoreMinimal.h"
#include "UE427AssetPath.h"

class UBlueprint;
class UObjectHierarchySerializer;
class FJsonObject;

class FCASJsonGraph
{
public:
	static bool Generate(UBlueprint* Blueprint, const TSharedPtr<FJsonObject>& AssetData, UObjectHierarchySerializer* Serializer) {
		FString Error;
		return Generate(Blueprint, AssetData, Serializer, Error);
	}
	static bool Generate(UBlueprint* Blueprint, const TSharedPtr<FJsonObject>& AssetData, UObjectHierarchySerializer* Serializer, FString& OutError);
	static void CollectDependencies(const TSharedPtr<FJsonObject>& AssetData, TArray<FString>& Packages);
};
