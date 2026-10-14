#pragma once

#include "CoreMinimal.h"
#include "UE427AssetPath.h"

class UEdGraph;

class FCASMacroReconstruction
{
public:
	static int32 Apply(UEdGraph* Graph);
	static int32 ApplyLoops(UEdGraph* Graph, bool bAggressive = false);
};
