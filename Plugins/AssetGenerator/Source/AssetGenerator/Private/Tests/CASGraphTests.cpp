#include "CoreMinimal.h"
#if ENGINE_MAJOR_VERSION >= 5
#include "GraphReconstruction/CASJsonGraph.h"
#include "UE427AssetPath.h"
#include "GraphReconstruction/MacroReconstruction.h"
#include "K2Node_CallFunction.h"
#include "K2Node_IfThenElse.h"
#include "K2Node_MacroInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Misc/AutomationTest.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/CompilerResultsLog.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/EngineTypes.h"
#include "K2Node_FunctionResult.h"
#include "EdGraph/EdGraph.h"
#include "EdGraphSchema_K2.h"
#include "UObject/StructOnScope.h"
#include "UObject/UnrealType.h"
#include "GameFramework/Actor.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCASStructSelectTest, "AssetGenerator.Graphs.StructSelect", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCASStructSelectTest::RunTest(const FString& Parameters)
{
	auto Blueprint = FKismetEditorUtilities::CreateBlueprint(UObject::StaticClass(), GetTransientPackage(), MakeUniqueObjectName(GetTransientPackage(), UBlueprint::StaticClass(), TEXT("CASStructSelect")), BPTYPE_Normal, UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass());
	FEdGraphPinType VectorType;
	VectorType.PinCategory = UEdGraphSchema_K2::PC_Struct;
	VectorType.PinSubCategoryObject = TBaseStructure<FVector>::Get();
	FBlueprintEditorUtils::AddMemberVariable(Blueprint, TEXT("FirstVector"), VectorType);
	FBlueprintEditorUtils::AddMemberVariable(Blueprint, TEXT("SecondVector"), VectorType);
	FEdGraphPinType ChoiceType;
	ChoiceType.PinCategory = UEdGraphSchema_K2::PC_Byte;
	ChoiceType.PinSubCategoryObject = StaticEnum<EAttachmentRule>();
	FBlueprintEditorUtils::AddMemberVariable(Blueprint, TEXT("Choice"), ChoiceType);
	FKismetEditorUtilities::CompileBlueprint(Blueprint, EBlueprintCompileOptions::SkipGarbageCollection);
	const FString Json = TEXT(R"json({"Children":[{"ObjectName":"ReadSelectedX","FunctionFlags":"335544320","ChildProperties":[{"FieldKind":"Property","ObjectClass":"DoubleProperty","ObjectName":"ReturnValue","PropertyFlags":"1408","ArrayDim":1,"RepNotifyFunc":"None","BlueprintReplicationCondition":0}],"Script":[{"Inst":"Return","Expression":{"Inst":"StructMemberContext","PropertyName":"X","StructExpression":{"Inst":"SwitchValue","Expression":{"Inst":"InstanceVariable","VariableName":"Choice"},"OffsetToSwitchEnd":100,"Cases":[{"CaseValue":{"Inst":"ByteConst","Value":0},"OffsetToNextCase":40,"CaseResult":{"Inst":"Context","Context":{"Inst":"Self"},"Expression":{"Inst":"InstanceVariable","VariableName":"FirstVector"}}},{"CaseValue":{"Inst":"ByteConst","Value":1},"OffsetToNextCase":70,"CaseResult":{"Inst":"Context","Context":{"Inst":"Self"},"Expression":{"Inst":"InstanceVariable","VariableName":"SecondVector"}}}],"DefaultResult":{"Inst":"Context","Context":{"Inst":"Self"},"Expression":{"Inst":"InstanceVariable","VariableName":"FirstVector"}}}},"StatementIndex":0},{"Inst":"EndOfScript","StatementIndex":101}]}]})json");
	TSharedPtr<FJsonObject> Data;
	FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Json), Data);
	TestTrue(TEXT("Generate struct-valued enum Select"), FCASJsonGraph::Generate(Blueprint, Data, nullptr));
	FCompilerResultsLog Results;
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
	FKismetEditorUtilities::CompileBlueprint(Blueprint, EBlueprintCompileOptions::SkipGarbageCollection, &Results);
	if (!TestEqual(TEXT("Struct Select compiles"), Results.NumErrors, 0)) return false;
	auto Object = Blueprint->GeneratedClass->GetDefaultObject();
	*FindFProperty<FStructProperty>(Blueprint->GeneratedClass, TEXT("FirstVector"))->ContainerPtrToValuePtr<FVector>(Object) = FVector(3, 0, 0);
	*FindFProperty<FStructProperty>(Blueprint->GeneratedClass, TEXT("SecondVector"))->ContainerPtrToValuePtr<FVector>(Object) = FVector(9, 0, 0);
	auto Choice = FindFProperty<FProperty>(Blueprint->GeneratedClass, TEXT("Choice"));
	auto Function = Blueprint->GeneratedClass->FindFunctionByName(TEXT("ReadSelectedX"));
	if (!TestNotNull(TEXT("Select function exists"), Function) || !TestNotNull(TEXT("Enum choice exists"), Choice)) return false;
	for (uint8 Value : {0, 1, 2})
	{
		if (auto Enum = CastField<FEnumProperty>(Choice)) Enum->GetUnderlyingProperty()->SetIntPropertyValue(Enum->ContainerPtrToValuePtr<void>(Object), static_cast<int64>(Value));
		else CastFieldChecked<FByteProperty>(Choice)->SetPropertyValue_InContainer(Object, Value);
		FStructOnScope Frame(Function);
		Object->ProcessEvent(Function, Frame.GetStructMemory());
		TestEqual(TEXT("Select preserves struct result for cases and default"), FindFProperty<FDoubleProperty>(Function, TEXT("ReturnValue"))->GetPropertyValue_InContainer(Frame.GetStructMemory()), Value == 1 ? 9.0 : 3.0);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCASMacroTest, "AssetGenerator.Graphs.MacroRecognition", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCASMacroTest::RunTest(const FString& Parameters)
{
	UBlueprint* Blueprint = FKismetEditorUtilities::CreateBlueprint(UObject::StaticClass(), GetTransientPackage(), MakeUniqueObjectName(GetTransientPackage(), UBlueprint::StaticClass(), TEXT("CASMacroCheck")), BPTYPE_Normal, UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass());
	UEdGraph* Graph = FBlueprintEditorUtils::CreateNewGraph(Blueprint, TEXT("MacroCheck"), UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());
	FBlueprintEditorUtils::AddUbergraphPage(Blueprint, Graph);
	auto Call = NewObject<UK2Node_CallFunction>(Graph);
	Call->SetFromFunction(UKismetSystemLibrary::StaticClass()->FindFunctionByName(TEXT("IsValid")));
	Graph->AddNode(Call);
	Call->AllocateDefaultPins();
	auto Branch = NewObject<UK2Node_IfThenElse>(Graph);
	Graph->AddNode(Branch);
	Branch->AllocateDefaultPins();
	auto External = NewObject<UK2Node_IfThenElse>(Graph);
	Graph->AddNode(External);
	External->AllocateDefaultPins();
	External->FindPin(UEdGraphSchema_K2::PN_Then)->MakeLinkTo(Branch->FindPin(UEdGraphSchema_K2::PN_Execute));
	Call->FindPin(UEdGraphSchema_K2::PN_ReturnValue)->MakeLinkTo(Branch->GetConditionPin());
	Call->FindPin(UEdGraphSchema_K2::PN_ReturnValue)->MakeLinkTo(External->GetConditionPin());
	TestEqual(TEXT("Shared validity result remains expanded"), FCASMacroReconstruction::Apply(Graph), 0);
	Call->FindPin(UEdGraphSchema_K2::PN_ReturnValue)->BreakLinkTo(External->GetConditionPin());
	TestEqual(TEXT("Exclusive validity branch becomes macro"), FCASMacroReconstruction::Apply(Graph), 1);
	TestEqual(TEXT("Macro reconstruction is idempotent"), FCASMacroReconstruction::Apply(Graph), 0);
	TestEqual(TEXT("Incoming execution preserved"), External->FindPin(UEdGraphSchema_K2::PN_Then)->LinkedTo.Num(), 1);
	TestTrue(TEXT("Execution now targets macro"), External->FindPin(UEdGraphSchema_K2::PN_Then)->LinkedTo.Num() == 1 && Cast<UK2Node_MacroInstance>(External->FindPin(UEdGraphSchema_K2::PN_Then)->LinkedTo[0]->GetOwningNode()) != nullptr);
	for (bool Valid : {false, true})
	{
		auto RuntimeBlueprint = FKismetEditorUtilities::CreateBlueprint(UObject::StaticClass(), GetTransientPackage(), MakeUniqueObjectName(GetTransientPackage(), UBlueprint::StaticClass(), TEXT("CASMacroRuntime")), BPTYPE_Normal, UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass());
		const FString Json = FString::Printf(TEXT(R"json({"Children":[{"ObjectName":"CheckValidity","FunctionFlags":"335544320","ChildProperties":[{"FieldKind":"Property","ObjectClass":"IntProperty","ObjectName":"ReturnValue","PropertyFlags":"1408","ArrayDim":1,"RepNotifyFunc":"None","BlueprintReplicationCondition":0}],"Script":[{"Inst":"JumpIfNot","Offset":20,"Condition":{"Inst":"CallMath","ContextClass":"/Script/Engine.KismetSystemLibrary","Function":"IsValid","Parameters":[{"Inst":"%s"}]},"StatementIndex":0},{"Inst":"Return","Expression":{"Inst":"IntConst","Value":7},"StatementIndex":10},{"Inst":"Return","Expression":{"Inst":"IntConst","Value":9},"StatementIndex":20},{"Inst":"EndOfScript","StatementIndex":30}]}]})json"), Valid ? TEXT("Self") : TEXT("NoObject"));
		TSharedPtr<FJsonObject> Data;
		FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Json), Data);
		TestTrue(TEXT("Build validity function"), FCASJsonGraph::Generate(RuntimeBlueprint, Data, nullptr));
		int32 Macros = 0;
		for (auto FunctionGraph : RuntimeBlueprint->FunctionGraphs)
			for (auto Node : FunctionGraph->Nodes)
				if (Cast<UK2Node_MacroInstance>(Node)) ++Macros;
		TestEqual(TEXT("Validity function contains real macro instance"), Macros, 1);
		FCompilerResultsLog Results;
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(RuntimeBlueprint);
		FKismetEditorUtilities::CompileBlueprint(RuntimeBlueprint, EBlueprintCompileOptions::SkipGarbageCollection, &Results);
		TestEqual(TEXT("Recovered macro compiles"), Results.NumErrors, 0);
		UFunction* Function = RuntimeBlueprint->GeneratedClass->FindFunctionByName(TEXT("CheckValidity"));
		if (Results.NumErrors == 0 && TestNotNull(TEXT("Validity function exists"), Function))
		{
			FStructOnScope Frame(Function);
			auto Return = FindFProperty<FIntProperty>(Function, TEXT("ReturnValue"));
			if (TestNotNull(TEXT("Validity return exists"), Return))
			{
				RuntimeBlueprint->GeneratedClass->GetDefaultObject()->ProcessEvent(Function, Frame.GetStructMemory());
				TestEqual(TEXT("Macro chooses correct valid/null execution path"), Return->GetPropertyValue_InContainer(Frame.GetStructMemory()), Valid ? 7 : 9);
			}
		}
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCASGraphReturnTest, "AssetGenerator.Graphs.ReturnValue", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCASGraphReturnTest::RunTest(const FString& Parameters)
{
	UBlueprint* Blueprint = FKismetEditorUtilities::CreateBlueprint(UObject::StaticClass(), GetTransientPackage(), MakeUniqueObjectName(GetTransientPackage(), UBlueprint::StaticClass(), TEXT("CASGraphCheck")), BPTYPE_Normal, UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass());
	const FString Json = TEXT(R"json({"Children":[{"ObjectName":"RecoveredValue","FunctionFlags":"335544320","ChildProperties":[{"FieldKind":"Property","ObjectClass":"IntProperty","ObjectName":"ReturnValue","PropertyFlags":"1408","ArrayDim":1,"RepNotifyFunc":"None","BlueprintReplicationCondition":0}],"Script":[{"Inst":"Return","Expression":{"Inst":"IntConst","Value":42},"StatementIndex":0},{"Inst":"EndOfScript","StatementIndex":6}]}]})json");
	TSharedPtr<FJsonObject> Data;
	TestTrue(TEXT("Parse fixture"), FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Json), Data));
	if (!Data || !Blueprint) return false;
	TestTrue(TEXT("Generate graph"), FCASJsonGraph::Generate(Blueprint, Data, nullptr));
	bool Found = false;
	for (UEdGraph* Graph : Blueprint->FunctionGraphs)
		for (UEdGraphNode* Node : Graph->Nodes)
			if (auto Result = Cast<UK2Node_FunctionResult>(Node))
				if (auto Pin = Result->FindPin(UEdGraphSchema_K2::PN_ReturnValue)) Found |= Pin->DefaultValue == TEXT("42");
	TestTrue(TEXT("Return constant retained"), Found);
	FCompilerResultsLog Results;
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
	FKismetEditorUtilities::CompileBlueprint(Blueprint, EBlueprintCompileOptions::SkipGarbageCollection, &Results);
	TestEqual(TEXT("Reconstructed graph compiles"), Results.NumErrors, 0);
	if (Results.NumErrors == 0)
	{
		UFunction* Function = Blueprint->GeneratedClass->FindFunctionByName(TEXT("RecoveredValue"));
		int32 ReturnValue = 0;
		if (TestNotNull(TEXT("Compiled function exists"), Function)) Blueprint->GeneratedClass->GetDefaultObject()->ProcessEvent(Function, &ReturnValue);
		TestEqual(TEXT("Recovered bytecode executes"), ReturnValue, 42);
	}
	TArray<FString> Packages;
	TSharedPtr<FJsonObject> References;
	FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(FString(TEXT(R"json({"Children":[{"Script":[{"Object":"/Game/Dependency.Dependency_C"},{"TableId":"/Game/Strings.Strings"},{"Value":"/Game/NotAReference.Asset"}]}]})json"))), References);
	FCASJsonGraph::CollectDependencies(References, Packages);
	TestEqual(TEXT("Object and string-table references schedule dependencies"), Packages.Num(), 2);
	TestTrue(TEXT("String table package retained"), Packages.Contains(TEXT("/Game/Strings")));
	TSharedPtr<FJsonObject> BranchData;
	const FString BranchJson = TEXT(R"json({"Children":[{"ObjectName":"RecoveredBranch","FunctionFlags":"335544320","ChildProperties":[{"FieldKind":"Property","ObjectClass":"BoolProperty","ObjectName":"Flag","PropertyFlags":"128","ArrayDim":1,"RepNotifyFunc":"None","BlueprintReplicationCondition":0},{"FieldKind":"Property","ObjectClass":"IntProperty","ObjectName":"ReturnValue","PropertyFlags":"1408","ArrayDim":1,"RepNotifyFunc":"None","BlueprintReplicationCondition":0}],"Script":[{"Inst":"JumpIfNot","Offset":20,"Condition":{"Inst":"LocalVariable","VariableName":"Flag"},"StatementIndex":0},{"Inst":"Return","Expression":{"Inst":"IntConst","Value":7},"StatementIndex":10},{"Inst":"Return","Expression":{"Inst":"IntConst","Value":9},"StatementIndex":20},{"Inst":"EndOfScript","StatementIndex":30}]}]})json");
	FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(BranchJson), BranchData);
	TestTrue(TEXT("Generate branch graph"), FCASJsonGraph::Generate(Blueprint, BranchData, nullptr));
	FCompilerResultsLog BranchResults;
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
	FKismetEditorUtilities::CompileBlueprint(Blueprint, EBlueprintCompileOptions::SkipGarbageCollection, &BranchResults);
	TestEqual(TEXT("Branch graph compiles"), BranchResults.NumErrors, 0);
	if (BranchResults.NumErrors == 0)
	{
		UFunction* Function = Blueprint->GeneratedClass->FindFunctionByName(TEXT("RecoveredBranch"));
		if (TestNotNull(TEXT("Branch function exists"), Function))
		{
			FStructOnScope Frame(Function);
			auto Flag = FindFProperty<FBoolProperty>(Function, TEXT("Flag"));
			auto Result = FindFProperty<FIntProperty>(Function, TEXT("ReturnValue"));
			if (TestNotNull(TEXT("Input exists"), Flag) && TestNotNull(TEXT("Return exists"), Result))
				for (bool Value : {false, true})
				{
					Flag->SetPropertyValue_InContainer(Frame.GetStructMemory(), Value);
					Blueprint->GeneratedClass->GetDefaultObject()->ProcessEvent(Function, Frame.GetStructMemory());
					TestEqual(TEXT("Branch selects its own return value"), Result->GetPropertyValue_InContainer(Frame.GetStructMemory()), Value ? 7 : 9);
				}
		}
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCASGraphEventTest, "AssetGenerator.Graphs.EventEntry", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCASGraphEventTest::RunTest(const FString& Parameters)
{
	UBlueprint* Blueprint = FKismetEditorUtilities::CreateBlueprint(AActor::StaticClass(), GetTransientPackage(), MakeUniqueObjectName(GetTransientPackage(), UBlueprint::StaticClass(), TEXT("CASEventCheck")), BPTYPE_Normal, UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass());
	FEdGraphPinType CounterType;
	CounterType.PinCategory = UEdGraphSchema_K2::PC_Int;
	FBlueprintEditorUtils::AddMemberVariable(Blueprint, TEXT("Counter"), CounterType);
	FKismetEditorUtilities::CompileBlueprint(Blueprint, EBlueprintCompileOptions::SkipGarbageCollection);
	const FString Json = TEXT(R"json({"Children":[{"ObjectName":"ReceiveBeginPlay","FunctionFlags":"134744064","ChildProperties":[],"Script":[{"Inst":"LocalFinalFunction","Function":"ExecuteUbergraph_Recovered","Parameters":[{"Inst":"IntConst","Value":10}],"StatementIndex":0},{"Inst":"Return","Expression":{"Inst":"Nothing"},"StatementIndex":15},{"Inst":"EndOfScript","StatementIndex":17}]},{"ObjectName":"ExecuteUbergraph_Recovered","FunctionFlags":"0","ChildProperties":[{"FieldKind":"Property","ObjectClass":"IntProperty","ObjectName":"EntryPoint","PropertyFlags":"128","ArrayDim":1,"RepNotifyFunc":"None","BlueprintReplicationCondition":0}],"Script":[{"Inst":"ComputedJump","OffsetExpression":{"Inst":"LocalVariable","VariableName":"EntryPoint"},"StatementIndex":0},{"Inst":"Let","Variable":{"Inst":"InstanceVariable","VariableName":"Counter"},"Expression":{"Inst":"CallMath","ContextClass":"/Script/Engine.KismetMathLibrary","Function":"Add_IntInt","Parameters":[{"Inst":"IntConst","Value":5},{"Inst":"IntConst","Value":8}]},"StatementIndex":10},{"Inst":"Return","Expression":{"Inst":"Nothing"},"StatementIndex":50},{"Inst":"EndOfScript","StatementIndex":52}]}]})json");
	TSharedPtr<FJsonObject> Data;
	FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Json), Data);
	TestTrue(TEXT("Generate event graph"), FCASJsonGraph::Generate(Blueprint, Data, nullptr));
	FCompilerResultsLog Results;
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
	FKismetEditorUtilities::CompileBlueprint(Blueprint, EBlueprintCompileOptions::SkipGarbageCollection, &Results);
	TestEqual(TEXT("Event graph compiles"), Results.NumErrors, 0);
	if (Results.NumErrors == 0)
	{
		UFunction* Function = Blueprint->GeneratedClass->FindFunctionByName(TEXT("ReceiveBeginPlay"));
		FIntProperty* Counter = FindFProperty<FIntProperty>(Blueprint->GeneratedClass, TEXT("Counter"));
		if (TestNotNull(TEXT("Event exists"), Function) && TestNotNull(TEXT("Member exists"), Counter))
		{
			UObject* Object = Blueprint->GeneratedClass->GetDefaultObject();
			Object->ProcessEvent(Function, nullptr);
			TestEqual(TEXT("Event runs native call and member assignment"), Counter->GetPropertyValue_InContainer(Object), 13);
		}
	}
	TestFalse(TEXT("Unchanged source preserves editor graphs"), FCASJsonGraph::Generate(Blueprint, Data, nullptr));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCASRealConversionTest, "AssetGenerator.Graphs.RealConversion", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCASRealConversionTest::RunTest(const FString& Parameters)
{
	for (int32 Mode = 0; Mode < 3; ++Mode)
	{
		const bool Narrow = Mode != 0;
		auto Blueprint = FKismetEditorUtilities::CreateBlueprint(UObject::StaticClass(), GetTransientPackage(), MakeUniqueObjectName(GetTransientPackage(), UBlueprint::StaticClass(), TEXT("CASRealConversion")), BPTYPE_Normal, UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass());
		const FString Json = FString::Printf(TEXT(R"json({"Children":[{"ObjectName":"ConvertValue","FunctionFlags":"335544320","ChildProperties":[{"FieldKind":"Property","ObjectClass":"%sProperty","ObjectName":"Input","PropertyFlags":"128","ArrayDim":1,"RepNotifyFunc":"None","BlueprintReplicationCondition":0},{"FieldKind":"Property","ObjectClass":"%sProperty","ObjectName":"ReturnValue","PropertyFlags":"1408","ArrayDim":1,"RepNotifyFunc":"None","BlueprintReplicationCondition":0}],"Script":[{"Inst":"Return","Expression":{"Inst":"PrimitiveCast","CastType":"%s","Expression":{"Inst":"LocalVariable","VariableName":"Input"}},"StatementIndex":0},{"Inst":"EndOfScript","StatementIndex":10}]}]})json"), Narrow ? TEXT("Double") : TEXT("Float"), Mode == 1 ? TEXT("Float") : TEXT("Double"), Narrow ? TEXT("DoubleToFloat") : TEXT("FloatToDouble"));
		TSharedPtr<FJsonObject> Data;
		FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Json), Data);
		if (Mode == 2)
		{
			auto Statement = Data->GetArrayField(TEXT("Children"))[0]->AsObject()->GetArrayField(TEXT("Script"))[0]->AsObject();
			auto Cast = MakeShared<FJsonObject>();
			Cast->SetStringField(TEXT("Inst"), TEXT("PrimitiveCast"));
			Cast->SetStringField(TEXT("CastType"), TEXT("FloatToDouble"));
			Cast->SetObjectField(TEXT("Expression"), Statement->GetObjectField(TEXT("Expression")));
			Statement->SetObjectField(TEXT("Expression"), Cast);
		}
		FString Error;
		TestTrue(TEXT("Generate real conversion"), FCASJsonGraph::Generate(Blueprint, Data, nullptr, Error));
		TestTrue(TEXT("No reconstruction error"), UE427IsEmpty(Error));
		int32 ExplicitConversions = 0;
		for (auto Graph : Blueprint->FunctionGraphs)
			for (auto Node : Graph->Nodes)
				if (auto Call = Cast<UK2Node_CallFunction>(Node); Call && Call->GetTargetFunction() && Call->GetTargetFunction()->GetName().StartsWith(TEXT("Conv_"))) ++ExplicitConversions;
		TestEqual(TEXT("No redundant explicit conversion"), ExplicitConversions, Mode == 2 ? 1 : 0);
		FCompilerResultsLog Results;
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
		FKismetEditorUtilities::CompileBlueprint(Blueprint, EBlueprintCompileOptions::SkipGarbageCollection, &Results);
		TestEqual(TEXT("Conversion compiles"), Results.NumErrors, 0);
		auto Function = Blueprint->GeneratedClass->FindFunctionByName(TEXT("ConvertValue"));
		if (!Function || Results.NumErrors) return false;
		FStructOnScope Frame(Function);
		auto Input = FindFProperty<FNumericProperty>(Function, TEXT("Input"));
		auto Output = FindFProperty<FNumericProperty>(Function, TEXT("ReturnValue"));
		if (!Input || !Output) return false;
		const double Value = 1.234567890123;
		Input->SetFloatingPointPropertyValue(Input->ContainerPtrToValuePtr<void>(Frame.GetStructMemory()), Value);
		Blueprint->GeneratedClass->GetDefaultObject()->ProcessEvent(Function, Frame.GetStructMemory());
		TestEqual(TEXT("Conversion preserves float rounding"), Output->GetFloatingPointPropertyValue(Output->ContainerPtrToValuePtr<void>(Frame.GetStructMemory())), static_cast<double>(static_cast<float>(Value)));
	}
	return true;
}

#endif

#endif
