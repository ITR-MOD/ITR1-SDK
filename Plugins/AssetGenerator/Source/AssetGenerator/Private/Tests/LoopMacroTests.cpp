#include "CoreMinimal.h"
#if ENGINE_MAJOR_VERSION >= 5
#include "GraphReconstruction/MacroReconstruction.h"
#include "UE427AssetPath.h"
#include "Misc/AutomationTest.h"
#include "EdGraph/EdGraph.h"
#include "EdGraphSchema_K2.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "K2Node_AssignmentStatement.h"
#include "K2Node_CallArrayFunction.h"
#include "K2Node_CallFunction.h"
#include "K2Node_CustomEvent.h"
#include "K2Node_IfThenElse.h"
#include "K2Node_MacroInstance.h"
#include "K2Node_TemporaryVariable.h"
#include "K2Node_VariableGet.h"
#include "K2Node_VariableSet.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Kismet2/CompilerResultsLog.h"
#include "UObject/UnrealType.h"
#include "EdGraphUtilities.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/OutputDeviceHelper.h"
#include "ScriptDisassembler.h"
#include "HAL/FileManager.h"

#if WITH_DEV_AUTOMATION_TESTS
namespace
{
struct FLoopFixture
{
	UBlueprint* Blueprint;
	UEdGraph* Graph;
	UK2Node_AssignmentStatement* Increment;
	UK2Node_CallFunction* Add;
	UK2Node_IfThenElse* Branch;
	UK2Node_VariableSet* Sum;
	UK2Node_VariableSet* Completed;
	UK2Node_TemporaryVariable* Counter;
	bool ForEach;

	template<class T> T* Node()
	{
		auto Result = NewObject<T>(Graph);
		Result->CreateNewGuid();
		Graph->AddNode(Result, false, false);
		return Result;
	}

	UEdGraphPin* Get(FName Name)
	{
		auto Result = Node<UK2Node_VariableGet>();
		Result->VariableReference.SetSelfMember(Name);
		Result->AllocateDefaultPins();
		return Result->FindPin(Name);
	}

	UK2Node_CallFunction* Math(FName Name, UEdGraphPin* A, UEdGraphPin* B = nullptr)
	{
		auto Result = Node<UK2Node_CallFunction>();
		Result->SetFromFunction(UKismetMathLibrary::StaticClass()->FindFunctionByName(Name));
		Result->AllocateDefaultPins();
		if (A) A->MakeLinkTo(Result->FindPin(TEXT("A")));
		if (B) B->MakeLinkTo(Result->FindPin(TEXT("B")));
		return Result;
	}

	UK2Node_AssignmentStatement* Assign(UEdGraphPin* Variable, UEdGraphPin* Value = nullptr)
	{
		auto Result = Node<UK2Node_AssignmentStatement>();
		Result->AllocateDefaultPins();
		Result->GetVariablePin()->PinType = Variable->PinType;
		Result->GetValuePin()->PinType = Variable->PinType;
		Result->GetValuePin()->DefaultValue = TEXT("0");
		Variable->MakeLinkTo(Result->GetVariablePin());
		if (Value) Value->MakeLinkTo(Result->GetValuePin());
		return Result;
	}

	UK2Node_VariableSet* Accumulate(FName Name, UEdGraphPin* Value)
	{
		auto Result = Node<UK2Node_VariableSet>();
		Result->VariableReference.SetSelfMember(Name);
		Result->AllocateDefaultPins();
		auto Plus = Math(TEXT("Add_IntInt"), Get(Name), Value);
		if (!Value) Plus->FindPin(TEXT("B"))->DefaultValue = TEXT("1");
		Plus->FindPin(UEdGraphSchema_K2::PN_ReturnValue)->MakeLinkTo(Result->FindPin(Name));
		return Result;
	}

	void Exec(UEdGraphNode* From, UEdGraphNode* To, FName Output = UEdGraphSchema_K2::PN_Then)
	{
		From->FindPin(Output)->MakeLinkTo(To->FindPin(UEdGraphSchema_K2::PN_Execute));
	}

	FLoopFixture(bool InForEach, bool DualIndex = false, bool Reverse = false, bool WithBreak = false) : ForEach(InForEach)
	{
		Blueprint = FKismetEditorUtilities::CreateBlueprint(UObject::StaticClass(), GetTransientPackage(), MakeUniqueObjectName(GetTransientPackage(), UBlueprint::StaticClass(), TEXT("CASLoopTest")), BPTYPE_Normal, UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass());
		FEdGraphPinType Int;
		Int.PinCategory = UEdGraphSchema_K2::PC_Int;
		for (auto Name : {TEXT("Sum"), TEXT("Count"), TEXT("IndexSum"), TEXT("Completed"), TEXT("First"), TEXT("Last")}) FBlueprintEditorUtils::AddMemberVariable(Blueprint, Name, Int);
		auto Array = Int;
		Array.ContainerType = EPinContainerType::Array;
		FBlueprintEditorUtils::AddMemberVariable(Blueprint, TEXT("Values"), Array);
		FKismetEditorUtilities::CompileBlueprint(Blueprint, EBlueprintCompileOptions::SkipGarbageCollection);
		Graph = FBlueprintEditorUtils::CreateNewGraph(Blueprint, TEXT("LoopGraph"), UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());
		FBlueprintEditorUtils::AddUbergraphPage(Blueprint, Graph);
		auto Entry = Node<UK2Node_CustomEvent>();
		Entry->CustomFunctionName = TEXT("RunLoop");
		Entry->AllocateDefaultPins();
		Counter = Node<UK2Node_TemporaryVariable>();
		Counter->VariableType = Int;
		Counter->AllocateDefaultPins();
		auto CounterPin = Counter->GetVariablePin();
		auto Init = Assign(CounterPin, ForEach ? nullptr : Get(TEXT("First")));
		Exec(Entry, Init);
		Branch = Node<UK2Node_IfThenElse>();
		Branch->AllocateDefaultPins();
		UEdGraphPin* Index = CounterPin;
		UEdGraphNode* BodyStart = Branch;
		UK2Node_AssignmentStatement* IndexInit = nullptr;
		if (DualIndex)
		{
			auto IndexNode = Node<UK2Node_TemporaryVariable>();
			IndexNode->VariableType = Int;
			IndexNode->AllocateDefaultPins();
			Index = IndexNode->GetVariablePin();
			IndexInit = Assign(Index);
			Exec(Init, IndexInit);
			Exec(IndexInit, Branch);
			auto Copy = Assign(Index, CounterPin);
			Exec(Branch, Copy);
			BodyStart = Copy;
		}
		else Exec(Init, Branch);
		UEdGraphPin* Bound;
		UEdGraphPin* Element = Index;
		if (ForEach)
		{
			auto ArrayPin = Get(TEXT("Values"));
			auto Length = Node<UK2Node_CallArrayFunction>();
			Length->SetFromFunction(UKismetArrayLibrary::StaticClass()->FindFunctionByName(TEXT("Array_Length")));
			Length->AllocateDefaultPins();
			Length->FindPin(TEXT("TargetArray"))->PinType = Array;
			ArrayPin->MakeLinkTo(Length->FindPin(TEXT("TargetArray")));
			Bound = Length->FindPin(UEdGraphSchema_K2::PN_ReturnValue);
			auto Item = Node<UK2Node_CallArrayFunction>();
			Item->SetFromFunction(UKismetArrayLibrary::StaticClass()->FindFunctionByName(TEXT("Array_Get")));
			Item->AllocateDefaultPins();
			Item->FindPin(TEXT("TargetArray"))->PinType = Array;
			Item->FindPin(TEXT("Item"))->PinType = Int;
			ArrayPin->MakeLinkTo(Item->FindPin(TEXT("TargetArray")));
			Index->MakeLinkTo(Item->FindPin(TEXT("Index")));
			Element = Item->FindPin(TEXT("Item"));
		}
		else Bound = Get(TEXT("Last"));
		if (Reverse)
		{
			auto LastIndex = Math(TEXT("Subtract_IntInt"), Bound);
			LastIndex->FindPin(TEXT("B"))->DefaultValue = TEXT("1");
			LastIndex->FindPin(UEdGraphSchema_K2::PN_ReturnValue)->MakeLinkTo(Init->GetValuePin());
			if (IndexInit)
			{
				auto Max = Math(TEXT("Max"), nullptr, LastIndex->FindPin(UEdGraphSchema_K2::PN_ReturnValue));
				Max->FindPin(TEXT("A"))->DefaultValue = TEXT("0");
				Max->FindPin(UEdGraphSchema_K2::PN_ReturnValue)->MakeLinkTo(IndexInit->GetValuePin());
			}
		}
		auto Compare = Math(Reverse ? TEXT("GreaterEqual_IntInt") : ForEach ? TEXT("Less_IntInt") : TEXT("LessEqual_IntInt"), CounterPin, Reverse ? nullptr : Bound);
		if (Reverse) Compare->FindPin(TEXT("B"))->DefaultValue = TEXT("0");
		Compare->FindPin(UEdGraphSchema_K2::PN_ReturnValue)->MakeLinkTo(Branch->GetConditionPin());
		Sum = Accumulate(TEXT("Sum"), Element);
		auto IndexSum = Accumulate(TEXT("IndexSum"), Index);
		auto Count = Accumulate(TEXT("Count"), nullptr);
		Exec(BodyStart, Sum);
		Exec(Sum, IndexSum);
		Exec(IndexSum, Count);
		Add = Math(Reverse ? TEXT("Subtract_IntInt") : TEXT("Add_IntInt"), CounterPin);
		Add->FindPin(TEXT("B"))->DefaultValue = TEXT("1");
		Increment = Assign(CounterPin, Add->FindPin(UEdGraphSchema_K2::PN_ReturnValue));
		Exec(Count, Increment);
		Exec(Increment, Branch);
		Completed = Accumulate(TEXT("Completed"), nullptr);
		Exec(Branch, Completed, UEdGraphSchema_K2::PN_Else);
		if (Reverse || WithBreak)
		{
			auto Plus = Sum->FindPin(TEXT("Sum"))->LinkedTo[0]->GetOwningNode();
			auto A = Plus->FindPin(TEXT("A"));
			auto Times = Math(TEXT("Multiply_IntInt"), A->LinkedTo[0]);
			Times->FindPin(TEXT("B"))->DefaultValue = TEXT("10");
			A->BreakAllPinLinks();
			Times->FindPin(UEdGraphSchema_K2::PN_ReturnValue)->MakeLinkTo(A);
		}
		if (WithBreak)
		{
			auto Flag = Node<UK2Node_TemporaryVariable>();
			Flag->VariableType.PinCategory = UEdGraphSchema_K2::PC_Boolean;
			Flag->AllocateDefaultPins();
			auto Reset = Assign(Flag->GetVariablePin());
			Reset->GetValuePin()->DefaultValue = TEXT("false");
			Entry->FindPin(UEdGraphSchema_K2::PN_Then)->BreakAllPinLinks();
			Exec(Entry, Reset);
			Exec(Reset, Init);
			auto Not = Math(TEXT("Not_PreBool"), Flag->GetVariablePin());
			auto And = Math(TEXT("BooleanAND"), Not->FindPin(UEdGraphSchema_K2::PN_ReturnValue), Compare->FindPin(UEdGraphSchema_K2::PN_ReturnValue));
			Branch->GetConditionPin()->BreakAllPinLinks();
			And->FindPin(UEdGraphSchema_K2::PN_ReturnValue)->MakeLinkTo(Branch->GetConditionPin());
			auto Check = Node<UK2Node_IfThenElse>();
			Check->AllocateDefaultPins();
			auto Equal = Math(TEXT("EqualEqual_IntInt"), Get(TEXT("Count")), Get(TEXT("Last")));
			Equal->FindPin(UEdGraphSchema_K2::PN_ReturnValue)->MakeLinkTo(Check->GetConditionPin());
			auto Break = Assign(Flag->GetVariablePin());
			Break->GetValuePin()->DefaultValue = TEXT("true");
			Count->FindPin(UEdGraphSchema_K2::PN_Then)->BreakAllPinLinks();
			Exec(Count, Check);
			Exec(Check, Break);
			Exec(Check, Increment, UEdGraphSchema_K2::PN_Else);
			Exec(Break, Increment);
		}
	}

	TArray<int32> Run(int32 First, int32 Last, const TArray<int32>& Values)
	{
		auto Object = Blueprint->GeneratedClass->GetDefaultObject();
		for (auto Name : {TEXT("Sum"), TEXT("Count"), TEXT("IndexSum"), TEXT("Completed")}) FindFProperty<FIntProperty>(Blueprint->GeneratedClass, Name)->SetPropertyValue_InContainer(Object, 0);
		FindFProperty<FIntProperty>(Blueprint->GeneratedClass, TEXT("First"))->SetPropertyValue_InContainer(Object, First);
		FindFProperty<FIntProperty>(Blueprint->GeneratedClass, TEXT("Last"))->SetPropertyValue_InContainer(Object, Last);
		auto Property = FindFProperty<FArrayProperty>(Blueprint->GeneratedClass, TEXT("Values"));
		FScriptArrayHelper Array(Property, Property->ContainerPtrToValuePtr<void>(Object));
		Array.Resize(Values.Num());
		for (int32 I = 0; I < Values.Num(); ++I) CastFieldChecked<FIntProperty>(Property->Inner)->SetPropertyValue(Array.GetRawPtr(I), Values[I]);
		Object->ProcessEvent(Blueprint->GeneratedClass->FindFunctionByName(TEXT("RunLoop")), nullptr);
		TArray<int32> Result;
		for (auto Name : {TEXT("Sum"), TEXT("Count"), TEXT("IndexSum"), TEXT("Completed")}) Result.Add(FindFProperty<FIntProperty>(Blueprint->GeneratedClass, Name)->GetPropertyValue_InContainer(Object));
		return Result;
	}
};
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCASLoopMacroTest, "AssetGenerator.Graphs.LoopMacros", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCASLoopMacroTest::RunTest(const FString& Parameters)
{
	for (int32 Variant = 0; Variant < 3; ++Variant)
	{
		FLoopFixture Fixture(Variant != 0, Variant == 2);
		FCompilerResultsLog Before;
		FKismetEditorUtilities::CompileBlueprint(Fixture.Blueprint, EBlueprintCompileOptions::SkipGarbageCollection, &Before);
		if (!TestEqual(TEXT("Expanded loop compiles"), Before.NumErrors, 0)) continue;
		const auto Baseline = Fixture.Run(3, 5, {2, 4, 8});
		TestTrue(TEXT("Expanded loop executes expected iterations"), Baseline == (Variant ? TArray<int32>{14, 3, 3, 1} : TArray<int32>{12, 3, 12, 1}));
		TestEqual(TEXT("Recognize loop"), FCASMacroReconstruction::ApplyLoops(Fixture.Graph), 1);
		TestEqual(TEXT("Do not duplicate loop macro"), FCASMacroReconstruction::ApplyLoops(Fixture.Graph), 0);
		FCompilerResultsLog After;
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Fixture.Blueprint);
		FKismetEditorUtilities::CompileBlueprint(Fixture.Blueprint, EBlueprintCompileOptions::SkipGarbageCollection, &After);
		if (!TestEqual(TEXT("Macro loop compiles"), After.NumErrors, 0)) continue;
		TestTrue(TEXT("Macro preserves indices, elements and completion"), Fixture.Run(3, 5, {2, 4, 8}) == Baseline);
		TestTrue(TEXT("Empty loop still completes once"), Fixture.Run(5, 3, {}) == TArray<int32>({0, 0, 0, 1}));
		TestTrue(TEXT("Single iteration completes once"), Fixture.Run(2, 2, {7}) == (Variant ? TArray<int32>{7, 1, 0, 1} : TArray<int32>{2, 1, 2, 1}));
		if (!Variant) TestTrue(TEXT("Negative indices preserved"), Fixture.Run(-2, 0, {}) == TArray<int32>({-3, 3, -3, 1}));
	}
	{
		FLoopFixture Fixture(false);
		auto Compare = Fixture.Branch->GetConditionPin()->LinkedTo[0]->GetOwningNode();
		Compare->FindPin(TEXT("A"))->BreakAllPinLinks();
		Fixture.Get(TEXT("Count"))->MakeLinkTo(Compare->FindPin(TEXT("A")));
		FCompilerResultsLog Before;
		FKismetEditorUtilities::CompileBlueprint(Fixture.Blueprint, EBlueprintCompileOptions::SkipGarbageCollection, &Before);
		if (TestEqual(TEXT("Expanded while loop compiles"), Before.NumErrors, 0))
		{
			const auto Baseline = Fixture.Run(0, 2, {});
			TestTrue(TEXT("Expanded while loop executes expected iterations"), Baseline == TArray<int32>({3, 3, 3, 1}));
			TestEqual(TEXT("Recognize while loop"), FCASMacroReconstruction::ApplyLoops(Fixture.Graph), 1);
			FCompilerResultsLog After;
			FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Fixture.Blueprint);
			FKismetEditorUtilities::CompileBlueprint(Fixture.Blueprint, EBlueprintCompileOptions::SkipGarbageCollection, &After);
			if (TestEqual(TEXT("While macro compiles"), After.NumErrors, 0))
			{
				TestTrue(TEXT("While condition is reevaluated after body"), Fixture.Run(0, 2, {}) == Baseline);
				TestTrue(TEXT("False while condition completes without body"), Fixture.Run(0, -1, {}) == TArray<int32>({0, 0, 0, 1}));
			}
		}
	}
	{
		FLoopFixture Fixture(false);
		Fixture.Add->FindPin(TEXT("B"))->DefaultValue = TEXT("2");
		TestEqual(TEXT("Non-unit stride stays expanded"), FCASMacroReconstruction::ApplyLoops(Fixture.Graph), 0);
	}
	{
		FLoopFixture Fixture(true);
		auto Read = Fixture.Accumulate(TEXT("Count"), Fixture.Counter->GetVariablePin());
		Fixture.Exec(Fixture.Completed, Read);
		TestEqual(TEXT("Counter read after loop stays expanded"), FCASMacroReconstruction::ApplyLoops(Fixture.Graph), 0);
	}
	{
		FLoopFixture Fixture(false);
		Fixture.Sum->FindPin(UEdGraphSchema_K2::PN_Then)->BreakAllPinLinks();
		Fixture.Exec(Fixture.Sum, Fixture.Completed);
		TestEqual(TEXT("Early exit stays expanded"), FCASMacroReconstruction::ApplyLoops(Fixture.Graph), 0);
	}
	{
		FLoopFixture Fixture(true, true);
		auto Event = Fixture.Node<UK2Node_CustomEvent>();
		Event->CustomFunctionName = TEXT("ExternalEntry");
		Event->AllocateDefaultPins();
		Fixture.Exec(Event, Fixture.Sum);
		TestEqual(TEXT("External body entry stays expanded"), FCASMacroReconstruction::ApplyLoops(Fixture.Graph), 0);
	}
	{
		FLoopFixture Fixture(true, true);
		auto Index = Fixture.Sum->FindPin(TEXT("Sum"))->LinkedTo[0]->GetOwningNode()->FindPin(TEXT("B"))->LinkedTo[0]->GetOwningNode()->FindPin(TEXT("Index"))->LinkedTo[0];
		auto Write = Fixture.Assign(Index);
		Fixture.Sum->FindPin(UEdGraphSchema_K2::PN_Then)->BreakAllPinLinks();
		Fixture.Exec(Fixture.Sum, Write);
		Fixture.Exec(Write, Fixture.Increment);
		TestEqual(TEXT("Array index mutation stays expanded"), FCASMacroReconstruction::ApplyLoops(Fixture.Graph), 0);
	}
	return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCASForEachVariantsTest, "AssetGenerator.Graphs.ForEachVariants", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCASForEachVariantsTest::RunTest(const FString& Parameters)
{
	for (bool Reverse : {false, true})
	{
		FLoopFixture Fixture(true, true, Reverse, !Reverse);
		FCompilerResultsLog Before;
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Fixture.Blueprint);
		FKismetEditorUtilities::CompileBlueprint(Fixture.Blueprint, EBlueprintCompileOptions::SkipGarbageCollection, &Before);
		if (!TestEqual(TEXT("Expanded variant compiles"), Before.NumErrors, 0)) continue;
		TArray<TArray<int32>> Expected;
		for (int32 Stop : {1, 2, 3, 4}) Expected.Add(Fixture.Run(0, Stop, {2, 4, 8}));
		TestTrue(TEXT("Iteration order and completion"), Expected[0] == (Reverse ? TArray<int32>({842, 3, 3, 1}) : TArray<int32>({2, 1, 0, 1})));
		TestTrue(TEXT("Full iteration order"), Expected[3] == TArray<int32>({Reverse ? 842 : 248, 3, 3, 1}));
		TestEqual(TEXT("Recognize foreach variant"), FCASMacroReconstruction::ApplyLoops(Fixture.Graph), 1);
		bool Found = false;
		for (auto Node : Fixture.Graph->Nodes)
			if (auto Macro = Cast<UK2Node_MacroInstance>(Node)) Found |= Macro->GetMacroGraph()->GetFName() == (Reverse ? TEXT("ReverseForEachLoop") : TEXT("ForEachLoopWithBreak"));
		TestTrue(TEXT("Correct engine macro selected"), Found);
		TestEqual(TEXT("Recovery is idempotent"), FCASMacroReconstruction::ApplyLoops(Fixture.Graph), 0);
		FCompilerResultsLog After;
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Fixture.Blueprint);
		FKismetEditorUtilities::CompileBlueprint(Fixture.Blueprint, EBlueprintCompileOptions::SkipGarbageCollection, &After);
		if (!TestEqual(TEXT("Recovered variant compiles"), After.NumErrors, 0)) continue;
		for (int32 Stop : {1, 2, 3, 4}) TestTrue(TEXT("First/middle/last/no break preserves execution"), Fixture.Run(0, Stop, {2, 4, 8}) == Expected[Stop - 1]);
		TestTrue(TEXT("Empty array completes once"), Fixture.Run(0, 1, {}) == TArray<int32>({0, 0, 0, 1}));
		TestTrue(TEXT("Single element completes once"), Fixture.Run(0, 1, {7}) == TArray<int32>({7, 1, 0, 1}));
		TestTrue(TEXT("Repeated call resets state"), Fixture.Run(0, 4, {2, 4, 8}) == Expected[3]);
	}
	for (int32 Case = 0; Case < 4; ++Case)
	{
		FLoopFixture Fixture(true, true, Case == 3, Case != 3);
		if (Case == 3) Fixture.Add->FindPin(TEXT("B"))->DefaultValue = TEXT("2");
		else
		{
			UK2Node_AssignmentStatement* Reset = nullptr;
			UK2Node_AssignmentStatement* Break = nullptr;
			for (auto Node : Fixture.Graph->Nodes)
				if (auto Set = Cast<UK2Node_AssignmentStatement>(Node); Set && Set->GetValuePin()->PinType.PinCategory == UEdGraphSchema_K2::PC_Boolean)
				{
					if (Set->GetValuePin()->DefaultValue == TEXT("false")) Reset = Set;
					else Break = Set;
				}
			if (Case == 0) Reset->GetValuePin()->DefaultValue = TEXT("true");
			if (Case == 1) Fixture.Math(TEXT("Not_PreBool"), Break->GetVariablePin()->LinkedTo[0]);
			if (Case == 2)
			{
				auto Extra = Fixture.Accumulate(TEXT("Count"), nullptr);
				Break->FindPin(UEdGraphSchema_K2::PN_Then)->BreakAllPinLinks();
				Fixture.Exec(Break, Extra);
				Fixture.Exec(Extra, Fixture.Increment);
			}
		}
		TestEqual(TEXT("Unsafe reset, escaping flag, post-break work or reverse stride stays expanded"), FCASMacroReconstruction::ApplyLoops(Fixture.Graph), 0);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCASAggressiveLoopTest, "AssetGenerator.Graphs.AggressiveLoops", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCASAggressiveLoopTest::RunTest(const FString& Parameters)
{
    for (int32 Variant = 0; Variant < 3; ++Variant)
    {
        FLoopFixture Fixture(true);
        if (Variant == 0)
        {
            auto Counter = Fixture.Add->FindPin(TEXT("A"))->LinkedTo[0];
            Fixture.Add->FindPin(TEXT("A"))->BreakAllPinLinks();
            Fixture.Add->FindPin(TEXT("A"))->DefaultValue = TEXT("1");
            Counter->MakeLinkTo(Fixture.Add->FindPin(TEXT("B")));
        }
        else if (Variant == 1)
        {
            auto Compare = Fixture.Branch->GetConditionPin()->LinkedTo[0]->GetOwningNode();
            Fixture.Math(TEXT("Add_IntInt"), Compare->FindPin(TEXT("B"))->LinkedTo[0]);
        }
        else
        {
            const auto Nodes = Fixture.Graph->Nodes;
            for (auto Node : Nodes)
                if (auto Call = Cast<UK2Node_CallFunction>(Node); Call && Call->GetTargetFunction() == UKismetArrayLibrary::StaticClass()->FindFunctionByName(TEXT("Array_Get")))
                {
                    auto Array = Call->FindPin(TEXT("TargetArray"));
                    Array->BreakAllPinLinks();
                    Fixture.Get(TEXT("Values"))->MakeLinkTo(Array);
                }
            TestEqual(TEXT("Conservative matcher preserves separate sources"), FCASMacroReconstruction::ApplyLoops(Fixture.Graph), 0);
        }
        FCompilerResultsLog Before;
        FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Fixture.Blueprint);
        FKismetEditorUtilities::CompileBlueprint(Fixture.Blueprint, EBlueprintCompileOptions::SkipGarbageCollection, &Before);
        if (!TestEqual(TEXT("Expanded aggressive fixture compiles"), Before.NumErrors, 0)) continue;
        auto Expected = Fixture.Run(0, 3, {2, 4, 8});
        TestEqual(TEXT("Aggressive foreach recovered"), FCASMacroReconstruction::ApplyLoops(Fixture.Graph, true), 1);
        FCompilerResultsLog After;
        FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Fixture.Blueprint);
        FKismetEditorUtilities::CompileBlueprint(Fixture.Blueprint, EBlueprintCompileOptions::SkipGarbageCollection, &After);
        if (TestEqual(TEXT("Recovered aggressive fixture compiles"), After.NumErrors, 0))
        {
            TestTrue(TEXT("Recovered execution matches expanded loop"), Fixture.Run(0, 3, {2, 4, 8}) == Expected);
            TestTrue(TEXT("Empty loop completes"), Fixture.Run(0, 3, {}) == TArray<int32>({0, 0, 0, 1}));
        }
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCASLoopReferenceTest, "AssetGenerator.Graphs.LoopReferences", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCASLoopReferenceTest::RunTest(const FString& Parameters)
{
	auto Library = LoadObject<UBlueprint>(nullptr, TEXT("/Engine/EditorBlueprintResources/StandardMacros.StandardMacros"));
	if (!TestNotNull(TEXT("Engine macro library"), Library)) return false;
	IFileManager::Get().MakeDirectory(*(FPaths::ProjectSavedDir() / TEXT("CASLoopReferences")), true);
	for (auto Definition : Library->MacroGraphs)
	{
		if (!Definition->GetName().Contains(TEXT("ForEach"))) continue;
		TSet<UObject*> Nodes;
		for (auto Node : Definition->Nodes) Nodes.Add(Node);
		FString Text;
		FEdGraphUtilities::ExportNodesToText(Nodes, Text);
		const FString Prefix = FPaths::ProjectSavedDir() / TEXT("CASLoopReferences") / Definition->GetName();
		TestTrue(TEXT("Save engine macro definition"), FFileHelper::SaveStringToFile(Text, *(Prefix + TEXT(".copy"))));
		FLoopFixture Fixture(true);
		TestEqual(TEXT("Prepare macro fixture"), FCASMacroReconstruction::ApplyLoops(Fixture.Graph), 1);
		const auto OriginalNodes = Fixture.Graph->Nodes;
		for (auto Node : OriginalNodes)
			if (auto Macro = Cast<UK2Node_MacroInstance>(Node))
			{
				auto Replacement = Fixture.Node<UK2Node_MacroInstance>();
				Replacement->SetMacroGraph(Definition);
				Replacement->AllocateDefaultPins();
				Replacement->ResolvedWildcardType = Macro->ResolvedWildcardType;
				for (auto P : Replacement->Pins)
					if (P->PinType.PinCategory == UEdGraphSchema_K2::PC_Wildcard)
						P->PinType.PinCategory = UEdGraphSchema_K2::PC_Int;
				for (auto P : Macro->Pins)
				{
					FName Name = P->PinName;
					if (Definition->GetFName() == TEXT("ReverseForEachLoop"))
					{
						if (Name == TEXT("Array Element")) Name = TEXT("ArrayElement");
						if (Name == TEXT("Array Index")) Name = TEXT("ArrayIndex");
					}
					if (auto Target = Replacement->FindPin(Name))
						for (auto Link : P->LinkedTo) Target->MakeLinkTo(Link);
				}
				Macro->BreakAllNodeLinks();
				Fixture.Graph->RemoveNode(Macro);
				if (auto Break = Replacement->FindPin(TEXT("Break")))
				{
					auto Check = Fixture.Node<UK2Node_IfThenElse>();
					Check->AllocateDefaultPins();
					auto Compare = Fixture.Math(TEXT("EqualEqual_IntInt"), Fixture.Get(TEXT("Count")), Fixture.Get(TEXT("Last")));
					Compare->FindPin(UEdGraphSchema_K2::PN_ReturnValue)->MakeLinkTo(Check->GetConditionPin());
					for (auto N : Fixture.Graph->Nodes)
						if (auto Set = Cast<UK2Node_VariableSet>(N); Set && Set->VariableReference.GetMemberName() == TEXT("Count")) Fixture.Exec(Set, Check);
					Check->GetThenPin()->MakeLinkTo(Break);
				}
			}
		FCompilerResultsLog Results;
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Fixture.Blueprint);
		FKismetEditorUtilities::CompileBlueprint(Fixture.Blueprint, EBlueprintCompileOptions::SkipGarbageCollection, &Results);
		if (!TestEqual(TEXT("Engine macro fixture compiles"), Results.NumErrors, 0)) continue;
		FStringOutputDevice Disassembly;
		FKismetBytecodeDisassembler Disassembler(Disassembly);
		for (TFieldIterator<UFunction> It(Fixture.Blueprint->GeneratedClass, EFieldIteratorFlags::ExcludeSuper); It; ++It)
			if (!UE427IsEmpty(It->Script)) Disassembler.DisassembleStructure(*It);
		TestTrue(TEXT("Save engine compiled Kismet"), FFileHelper::SaveStringToFile(Disassembly.Replace(TEXT("Label_"), TEXT("\nLabel_")), *(Prefix + TEXT(".kismet.txt"))));
	}
	return true;
}
#endif

#endif
