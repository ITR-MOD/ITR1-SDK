#pragma once

#include "CoreMinimal.h"
#include "UE427AssetPath.h"
#include "BytecodeReader.h"

class UBlueprint;
class UEdGraph;
class UEdGraphNode;
class UEdGraphPin;
class UK2Node;
class UBlueprintGeneratedClass;
class UPackage;

class FCASGraphBuilder
{
public:
	FCASGraphBuilder();
	void AddComponentDelegateBinding(FName FunctionName, FName ComponentName, FName DelegateName) { ComponentDelegateBindings.Add(FunctionName, TPair<FName, FName>(ComponentName, DelegateName)); }
	void BuildGraphs(UBlueprint* Blueprint, UClass* SourceClass, const TArray<FCASDecompiledFunction>& Functions);

	UBlueprint* BuildBlueprint(
		UClass* OriginalClass,
		const TArray<FCASDecompiledFunction>& Functions,
		UPackage* TargetPackage,
		const FString& BlueprintName);

	UBlueprint* LiveBuildBlueprint(
		UClass* OriginalClass,
		const TArray<FCASDecompiledFunction>& Functions,
		UBlueprintGeneratedClass* ExistingBPGC);

	UBlueprint* BuildChildBlueprint(
		UClass* OriginalClass,
		const TArray<FCASDecompiledFunction>& Functions,
		UPackage* TargetPackage,
		const FString& BlueprintName);

	const TArray<FString>& GetWarnings() const { return Warnings; }

	struct FPinSnapshot
	{
		FGuid OwningNodeGuid;
		FName PinName;
		EEdGraphPinDirection Direction;
		TArray<TPair<FGuid, FName>> LinkedPinIds; 
		FString DefaultValue;
		TWeakObjectPtr<UObject> DefaultObject;
		FText DefaultTextValue;
	};

	static TArray<FPinSnapshot> SnapshotAllConnections(UBlueprint* BP);

	static void RestoreConnectionsFromSnapshot(UBlueprint* BP, const TArray<FPinSnapshot>& Snapshot);

	static void RunPostRefreshFixups(UBlueprint* BP);

	struct FEventRefSnapshot
	{
		FGuid NodeGuid;
		FName EventName;
		TWeakObjectPtr<UClass> DeclaringClass;
	};

	static TArray<FEventRefSnapshot> SnapshotEventReferences(UBlueprint* BP);

	static void RestoreEventReferences(UBlueprint* BP, const TArray<FEventRefSnapshot>& Snapshot);

private:
	TMap<FName, TPair<FName, FName>> ComponentDelegateBindings;

	bool BuildBlueprintCore(UBlueprint* BP, UClass* OriginalClass, const TArray<FCASDecompiledFunction>& Functions, bool bIsLiveUncook = false, bool bIsChildClass = false);

	void SetupVariables(UBlueprint* BP, UClass* OriginalClass);
	void SetupInterfaces(UBlueprint* BP, UClass* OriginalClass);
	void SetupComponents(UBlueprint* BP, UClass* OriginalClass);
	void SetupCDODefaults(UBlueprint* BP, UClass* OriginalClass);

	void BuildEventGraph(UBlueprint* BP, UEdGraph* EventGraph,
		const FCASDecompiledFunction& UbergraphFunc,
		const TArray<FCASDecompiledFunction>& EventStubs,
		UClass* OriginalClass);

	void BuildFunctionGraph(UBlueprint* BP, const FCASDecompiledFunction& Func, UClass* OriginalClass);

	void EmitStatements(
		UEdGraph* Graph,
		const TArray<TSharedPtr<FCASDecompiledExpr>>& Statements,
		int32 StartIndex, int32 EndIndex,
		int32 BaseX, int32 BaseY,
		UEdGraphPin* IncomingExecPin,
		const TSet<int32>* pReachableStmts = nullptr,
		int32 EventEntryStmtIdx = -1);

	UEdGraphNode* CreateNodeForExpr(
		UEdGraph* Graph,
		TSharedPtr<FCASDecompiledExpr> Expr,
		int32& X, int32& Y);

	UEdGraphPin* ResolveDataExpr(
		UEdGraph* Graph,
		TSharedPtr<FCASDecompiledExpr> Expr,
		int32& X, int32& Y);

	static bool TryConnect(UEdGraphPin* A, UEdGraphPin* B);

	static bool ForceConnect(UEdGraphPin* A, UEdGraphPin* B);

	bool ApplyTempLiteralToPin(TSharedPtr<FCASDecompiledExpr> Expr, UEdGraphPin* Pin);
	static UEdGraphPin* FindExecPin(UEdGraphNode* Node, EEdGraphPinDirection Direction, FName PinName = NAME_None);
	static UEdGraphPin* FindDataPin(UEdGraphNode* Node, EEdGraphPinDirection Direction, FName PinName);
	static UEdGraphPin* FindFirstDataOutputPin(UEdGraphNode* Node);
	static FName GetPinNameForFunctionParam(UFunction* Function, int32 ParamIndex);

	static void FixStructPinDefaults(UEdGraphNode* Node);

	static void SanitizeGraphConnections(UEdGraph* Graph);

	TArray<FString> Warnings;
	UClass* CurrentOriginalClass = nullptr;
	UFunction* CurrentFunctionSignature = nullptr;
	bool bBuiltReturnNodes = false;
	bool bIsRedirectedVirtualCall = false; 
	bool bBuildingChildClass = false; 

	TSet<FName> EventStubParamNames;

	TMap<FName, UEdGraphPin*> LocalVarToPinMap;

	TMap<FName, UEdGraphPin*> PersistentFrameVarMap;

	TMap<FName, TSharedPtr<FCASDecompiledExpr>> TempLiteralMap;

	TMap<FName, TSharedPtr<FCASDecompiledExpr>> OutParamLiteralMap;

	struct FStructMemberWrite
	{
		UScriptStruct* StructType = nullptr;
		TMap<FName, TSharedPtr<FCASDecompiledExpr>> MemberValues;
	};
	TMap<FName, FStructMemberWrite> StructMemberAssignMap;

	TSet<FName> MultiAssignedTempVars;

	TMap<FName, UEdGraphPin*> MultiAssignedTempPins;

	UEdGraphNode* LastCastNode = nullptr;

	UEdGraphPin* LastFunctionExecOut = nullptr;

	TArray<UEdGraphPin*> ReturnExecSources;

	TArray<TPair<UEdGraphPin*, FName>> DeferredDataWires;
	TArray<TPair<UEdGraphPin*, FName>> DeferredEventParameterWires;

	struct FDeferredPinDefault
	{
		TWeakObjectPtr<UEdGraphNode> Node;
		FName PinName;
		FString DefaultValue;
		TWeakObjectPtr<UObject> DefaultObject;
	};
	TArray<FDeferredPinDefault> DeferredPinDefaults;

	void ClearLocalVarMap()
	{
		LocalVarToPinMap.Reset();
		TempLiteralMap.Reset();
		OutParamLiteralMap.Reset();
		StructMemberAssignMap.Reset();
		MultiAssignedTempVars.Reset();
		MultiAssignedTempPins.Reset();
		DeferredDataWires.Reset();
		LastCastNode = nullptr;
		LastFunctionExecOut = nullptr;
		ReturnExecSources.Reset();
	}

	static constexpr int32 NodeSpacingX = 350;
	static constexpr int32 NodeSpacingY = 200;
	static constexpr int32 DataNodeOffsetY = -150;

	void Warn(const FString& Message);
};
