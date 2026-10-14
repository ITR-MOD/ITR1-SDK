#include "MacroReconstruction.h"
#include "UE427AssetPath.h"
#include "EdGraph/EdGraph.h"
#include "EdGraphSchema_K2.h"
#include "Engine/Blueprint.h"
#include "K2Node_AssignmentStatement.h"
#include "K2Node_CallFunction.h"
#include "K2Node_DynamicCast.h"
#include "K2Node_IfThenElse.h"
#include "K2Node_MacroInstance.h"
#include "K2Node_GetArrayItem.h"
#include "K2Node_Knot.h"
#include "K2Node_TemporaryVariable.h"
#include "K2Node_VariableSet.h"
#include "K2Node_VariableGet.h"
#include "K2Node_Self.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(LogCASLoopMacros, Log, All);

namespace
{
UEdGraphPin* Pin(UEdGraphNode* Node, FName Name)
{
	return Node ? Node->FindPin(Name) : nullptr;
}

UEdGraphPin* Source(UEdGraphPin* Input)
{
	return Input && Input->LinkedTo.Num() == 1 ? Input->LinkedTo[0] : nullptr;
}

bool Linked(UEdGraphPin* A, UEdGraphPin* B)
{
	return A && B && Source(A) == B;
}

bool Literal(UEdGraphPin* Input, int32 Value)
{
	return Input && UE427IsEmpty(Input->LinkedTo) && (Input->DefaultValue == FString::FromInt(Value) || (Value == 0 && UE427IsEmpty(Input->DefaultValue)));
}

bool EquivalentSource(UEdGraphPin* A, UEdGraphPin* B, int32 Depth = 0)
{
	if (A == B) return A != nullptr;
	if (!A || !B || Depth > 16 || A->PinName != B->PinName || A->PinType != B->PinType) return false;
	auto Left = Cast<UK2Node>(A->GetOwningNode());
	auto Right = Cast<UK2Node>(B->GetOwningNode());
	if (!Left || !Right || Left->GetClass() != Right->GetClass() || !Left->IsNodePure() || !Right->IsNodePure()) return false;
	if (auto Get = Cast<UK2Node_VariableGet>(Left))
	{
		auto Other = CastChecked<UK2Node_VariableGet>(Right);
		if (Get->GetVarName() != Other->GetVarName() || Get->VariableReference.IsSelfContext() != Other->VariableReference.IsSelfContext() ||
			Get->VariableReference.GetMemberParentClass() != Other->VariableReference.GetMemberParentClass()) return false;
	}
	else if (auto Call = Cast<UK2Node_CallFunction>(Left))
	{
		auto Function = Call->GetTargetFunction();
		if (!Function || Function != CastChecked<UK2Node_CallFunction>(Right)->GetTargetFunction() || !Function->HasAnyFunctionFlags(FUNC_BlueprintPure) || Function->HasMetaData(TEXT("Latent"))) return false;
	}
	else if (!Cast<UK2Node_Self>(Left)) return false;
	if (Left->Pins.Num() != Right->Pins.Num()) return false;
	for (auto Input : Left->Pins)
	{
		if (Input->Direction != EGPD_Input) continue;
		auto Other = Right->FindPin(Input->PinName, EGPD_Input);
		if (!Other || Input->PinType != Other->PinType || Input->LinkedTo.Num() != Other->LinkedTo.Num()) return false;
		if (UE427IsEmpty(Input->LinkedTo))
		{
			if (Input->DefaultValue != Other->DefaultValue || Input->DefaultObject != Other->DefaultObject || !Input->DefaultTextValue.EqualTo(Other->DefaultTextValue)) return false;
		}
		else if (Input->LinkedTo.Num() != 1 || !EquivalentSource(Source(Input), Source(Other), Depth + 1)) return false;
	}
	return true;
}

UK2Node_CallFunction* NativeCall(UEdGraphPin* Output, UClass* Owner, FName Name)
{
	auto Call = Output ? Cast<UK2Node_CallFunction>(Output->GetOwningNode()) : nullptr;
	return Call && Call->GetTargetFunction() == Owner->FindFunctionByName(Name) && Output == Pin(Call, UEdGraphSchema_K2::PN_ReturnValue) ? Call : nullptr;
}

bool Exclusive(UEdGraphPin* Output, UEdGraphPin* Input)
{
	return Linked(Output, Input) && Linked(Input, Output);
}

bool BodyNode(UEdGraphNode* Node)
{
	if (Cast<UK2Node_IfThenElse>(Node) || Cast<UK2Node_VariableSet>(Node) || Cast<UK2Node_AssignmentStatement>(Node) || Cast<UK2Node_DynamicCast>(Node)) return true;
	if (auto Call = Cast<UK2Node_CallFunction>(Node))
		return Call->GetTargetFunction() && !Call->GetTargetFunction()->HasMetaData(TEXT("Latent"));
	if (auto Macro = Cast<UK2Node_MacroInstance>(Node))
		return Macro->GetMacroGraph() && Macro->GetMacroGraph()->GetPathName() == TEXT("/Engine/EditorBlueprintResources/StandardMacros.StandardMacros:IsValid");
	return false;
}

bool GatherBody(UEdGraphPin* Start, UEdGraphPin* End, const TSet<UEdGraphNode*>& Control, TSet<UEdGraphNode*>& Body, bool AllowExternalEnd = false)
{
	TSet<UEdGraphNode*> Active;
	TFunction<bool(UEdGraphPin*)> Walk = [&](UEdGraphPin* Output)
	{
		if (!Output || Output->LinkedTo.Num() != 1) return false;
		auto Input = Output->LinkedTo[0];
		if (Input == End) return true;
		auto Node = Input->GetOwningNode();
		if (Control.Contains(Node) || Active.Contains(Node) || !BodyNode(Node)) return false;
		if (Body.Contains(Node)) return true;
		Active.Add(Node);
		int32 Outputs = 0;
		for (auto P : Node->Pins)
			if (P->Direction == EGPD_Output && P->PinType.PinCategory == UEdGraphSchema_K2::PC_Exec)
			{
				++Outputs;
				if (!Walk(P)) return false;
			}
		Active.Remove(Node);
		if (!Outputs) return false;
		Body.Add(Node);
		return true;
	};
	if (!Walk(Start)) return false;
	for (auto Node : Body)
		for (auto P : Node->Pins)
			if (P->Direction == EGPD_Input && P->PinType.PinCategory == UEdGraphSchema_K2::PC_Exec)
				for (auto Link : P->LinkedTo)
					if (Link != Start && !Body.Contains(Link->GetOwningNode())) return false;
	if (!AllowExternalEnd)
		for (auto Link : End->LinkedTo)
			if (Link != Start && !Body.Contains(Link->GetOwningNode())) return false;
	return true;
}

bool UsedOnlyInBody(UEdGraphPin* Output, const TSet<UEdGraphNode*>& Control, const TSet<UEdGraphNode*>& Body, TSet<UEdGraphNode*>& Visiting)
{
	for (auto Input : Output->LinkedTo)
	{
		auto Node = Input->GetOwningNode();
		if (Control.Contains(Node)) continue;
		if (auto Assignment = Cast<UK2Node_AssignmentStatement>(Node); Assignment && Input == Assignment->GetVariablePin()) return false;
		if (Input->PinType.bIsReference && !Input->PinType.bIsConst) return false;
		if (Body.Contains(Node)) continue;
		auto K2 = Cast<UK2Node>(Node);
		if (!K2 || !K2->IsNodePure() || Visiting.Contains(Node)) return false;
		Visiting.Add(Node);
		for (auto P : Node->Pins)
			if (P->Direction == EGPD_Output && !UsedOnlyInBody(P, Control, Body, Visiting)) return false;
		Visiting.Remove(Node);
	}
	return true;
}

bool IndependentInput(UEdGraphPin* Input, const TSet<UEdGraphNode*>& Removed, TSet<UEdGraphNode*>& Seen)
{
	for (auto Output : Input->LinkedTo)
	{
		auto Node = Output->GetOwningNode();
		if (Removed.Contains(Node)) return false;
		if (Seen.Contains(Node)) continue;
		Seen.Add(Node);
		for (auto P : Node->Pins)
			if (P->Direction == EGPD_Input && P->PinType.PinCategory != UEdGraphSchema_K2::PC_Exec && !IndependentInput(P, Removed, Seen)) return false;
	}
	return true;
}

bool RecoverLoop(UEdGraph* Graph, UK2Node_IfThenElse* Branch, bool bAggressive = false)
{
	auto ConditionInput = Branch->GetConditionPin();
	auto And = NativeCall(Source(ConditionInput), UKismetMathLibrary::StaticClass(), TEXT("BooleanAND"));
	UK2Node_CallFunction* Not = nullptr;
	UEdGraphPin* Flag = nullptr;
	if (And)
	{
		if (!Exclusive(Pin(And, UEdGraphSchema_K2::PN_ReturnValue), ConditionInput)) return false;
		auto FlagInput = Pin(And, TEXT("A"));
        auto CompareInput = Pin(And, TEXT("B"));
        Not = NativeCall(Source(FlagInput), UKismetMathLibrary::StaticClass(), TEXT("Not_PreBool"));
		if (!Not && bAggressive)
        {
            Swap(FlagInput, CompareInput);
            Not = NativeCall(Source(FlagInput), UKismetMathLibrary::StaticClass(), TEXT("Not_PreBool"));
        }
        if (!Not || !Exclusive(Pin(Not, UEdGraphSchema_K2::PN_ReturnValue), FlagInput)) return false;
		Flag = Source(Pin(Not, TEXT("A")));
		if (!Flag || !Cast<UK2Node_TemporaryVariable>(Flag->GetOwningNode()) || Flag->PinType.PinCategory != UEdGraphSchema_K2::PC_Boolean || Flag->PinType.IsContainer()) return false;
		ConditionInput = CompareInput;
	}
	auto Compare = NativeCall(Source(ConditionInput), UKismetMathLibrary::StaticClass(), TEXT("LessEqual_IntInt"));
	const bool ForEach = Compare == nullptr;
	if (ForEach) Compare = NativeCall(Source(ConditionInput), UKismetMathLibrary::StaticClass(), TEXT("Less_IntInt"));
    bool bSwappedComparison = false;
    if (!Compare && bAggressive)
    {
        Compare = NativeCall(Source(ConditionInput), UKismetMathLibrary::StaticClass(), TEXT("Greater_IntInt"));
        bSwappedComparison = Compare != nullptr;
    }
    const bool Reverse = !Compare && NativeCall(Source(ConditionInput), UKismetMathLibrary::StaticClass(), TEXT("GreaterEqual_IntInt"));
	if (Reverse) Compare = NativeCall(Source(ConditionInput), UKismetMathLibrary::StaticClass(), TEXT("GreaterEqual_IntInt"));
	if (!Compare || !Exclusive(Pin(Compare, UEdGraphSchema_K2::PN_ReturnValue), ConditionInput)) return false;
	if (And && (!ForEach || Reverse)) return false;
    auto CounterInput = Pin(Compare, bSwappedComparison ? TEXT("B") : TEXT("A"));
    auto LimitInput = Pin(Compare, bSwappedComparison ? TEXT("A") : TEXT("B"));
    auto Counter = Source(CounterInput);
	if (!Counter || !Cast<UK2Node_TemporaryVariable>(Counter->GetOwningNode()) || Counter->PinType.PinCategory != UEdGraphSchema_K2::PC_Int || Counter->PinType.IsContainer()) return false;
	UK2Node_AssignmentStatement* Init = nullptr;
	UK2Node_AssignmentStatement* Increment = nullptr;
	UK2Node_CallFunction* Add = nullptr;
	for (auto Link : Counter->LinkedTo)
	{
		auto Assignment = Cast<UK2Node_AssignmentStatement>(Link->GetOwningNode());
		if (!Assignment || Link != Assignment->GetVariablePin()) continue;
		auto Candidate = NativeCall(Source(Assignment->GetValuePin()), UKismetMathLibrary::StaticClass(), Reverse ? TEXT("Subtract_IntInt") : TEXT("Add_IntInt"));
		if (Candidate && ((Linked(Pin(Candidate, TEXT("A")), Counter) && Literal(Pin(Candidate, TEXT("B")), 1)) || (bAggressive && !Reverse && Linked(Pin(Candidate, TEXT("B")), Counter) && Literal(Pin(Candidate, TEXT("A")), 1))))
		{
			if (Increment) return false;
			Increment = Assignment;
			Add = Candidate;
		}
		else { if (Init) return false; Init = Assignment; }
	}
	if (!Init || !Increment || !Exclusive(Pin(Add, UEdGraphSchema_K2::PN_ReturnValue), Increment->GetValuePin()) ||
		!Linked(Pin(Increment, UEdGraphSchema_K2::PN_Then), Pin(Branch, UEdGraphSchema_K2::PN_Execute)) || UE427IsEmpty(Pin(Init, UEdGraphSchema_K2::PN_Execute)->LinkedTo)) return false;
	TSet<UEdGraphNode*> Removed = {Branch, Compare, Counter->GetOwningNode(), Init, Increment, Add};
	auto BodyStart = Pin(Branch, UEdGraphSchema_K2::PN_Then);
	auto InitEnd = Pin(Init, UEdGraphSchema_K2::PN_Then);
	UEdGraphPin* Index = Counter;
	UK2Node_CallFunction* Length = nullptr;
	TArray<UEdGraphPin*> Elements;
	UEdGraphPin* ArrayInput = nullptr;
	if (ForEach)
	{
		auto LastIndex = Reverse ? NativeCall(Source(Init->GetValuePin()), UKismetMathLibrary::StaticClass(), TEXT("Subtract_IntInt")) : nullptr;
		if (Reverse && (!LastIndex || !Literal(Pin(LastIndex, TEXT("B")), 1) || !Literal(LimitInput, 0))) return false;
		auto LengthUse = Reverse ? Pin(LastIndex, TEXT("A")) : LimitInput;
		Length = NativeCall(Source(LengthUse), UKismetArrayLibrary::StaticClass(), TEXT("Array_Length"));
		if (!Length || (!bAggressive && !Exclusive(Pin(Length, UEdGraphSchema_K2::PN_ReturnValue), LengthUse)) || (!Reverse && !Literal(Init->GetValuePin(), 0))) return false;
		ArrayInput = Pin(Length, TEXT("TargetArray"));
		if (!Source(ArrayInput)) return false;
		if (Exclusive(Pin(Length, UEdGraphSchema_K2::PN_ReturnValue), LengthUse)) Removed.Add(Length);
		if (LastIndex) Removed.Add(LastIndex);
		if (!Linked(InitEnd, Pin(Branch, UEdGraphSchema_K2::PN_Execute)))
		{
			auto Copy = Source(BodyStart) ? Cast<UK2Node_AssignmentStatement>(Source(BodyStart)->GetOwningNode()) : nullptr;
			auto OutputInit = Source(InitEnd) ? Cast<UK2Node_AssignmentStatement>(Source(InitEnd)->GetOwningNode()) : nullptr;
			if (!Copy || !OutputInit || !Exclusive(BodyStart, Pin(Copy, UEdGraphSchema_K2::PN_Execute)) || !Exclusive(InitEnd, Pin(OutputInit, UEdGraphSchema_K2::PN_Execute)) ||
				!Linked(Copy->GetValuePin(), Counter)) return false;
			if (Reverse)
			{
				auto Max = NativeCall(Source(OutputInit->GetValuePin()), UKismetMathLibrary::StaticClass(), TEXT("Max"));
				if (!Max || !Literal(Pin(Max, TEXT("A")), 0) || !Linked(Pin(Max, TEXT("B")), Pin(LastIndex, UEdGraphSchema_K2::PN_ReturnValue)) ||
					!Exclusive(Pin(Max, UEdGraphSchema_K2::PN_ReturnValue), OutputInit->GetValuePin())) return false;
				Removed.Add(Max);
			}
			else if (!Literal(OutputInit->GetValuePin(), 0)) return false;
			Index = Source(Copy->GetVariablePin());
			if (!Index || Index == Counter || !Cast<UK2Node_TemporaryVariable>(Index->GetOwningNode()) || !Linked(OutputInit->GetVariablePin(), Index)) return false;
			Removed.Append({Copy, OutputInit, Index->GetOwningNode()});
			InitEnd = Pin(OutputInit, UEdGraphSchema_K2::PN_Then);
			BodyStart = Pin(Copy, UEdGraphSchema_K2::PN_Then);
		}
		if (LastIndex)
			for (auto Link : Pin(LastIndex, UEdGraphSchema_K2::PN_ReturnValue)->LinkedTo)
				if (!Removed.Contains(Link->GetOwningNode())) return false;
		for (auto Link : Index->LinkedTo)
		{
            if (bAggressive)
            {
                auto GetItem = Cast<UK2Node_GetArrayItem>(Link->GetOwningNode());
                if (GetItem && Link == GetItem->GetIndexPin())
                {
                    if (!EquivalentSource(Source(GetItem->GetTargetArrayPin()), Source(ArrayInput)) || GetItem->GetResultPin()->PinType.bIsReference) return false;
                    Elements.Add(GetItem->GetResultPin());
                    Removed.Add(GetItem);
                    continue;
                }
            }
			auto Get = Cast<UK2Node_CallFunction>(Link->GetOwningNode());
			if (!Get || Get->GetTargetFunction() != UKismetArrayLibrary::StaticClass()->FindFunctionByName(TEXT("Array_Get")) || Link != Pin(Get, TEXT("Index"))) continue;
			if (!(bAggressive ? EquivalentSource(Source(Pin(Get, TEXT("TargetArray"))), Source(ArrayInput)) : Linked(Pin(Get, TEXT("TargetArray")), Source(ArrayInput))) || !Pin(Get, TEXT("Item"))) return false;
			Elements.Add(Pin(Get, TEXT("Item")));
			Removed.Add(Get);
		}
	}
	if (!Linked(InitEnd, Pin(Branch, UEdGraphSchema_K2::PN_Execute)) || Pin(Branch, UEdGraphSchema_K2::PN_Execute)->LinkedTo.Num() != 2) return false;
	UK2Node_AssignmentStatement* FlagInit = nullptr;
	TArray<UK2Node_AssignmentStatement*> Breaks;
	if (Flag)
	{
		for (auto Link : Flag->LinkedTo)
		{
			if (Link == Pin(Not, TEXT("A"))) continue;
			auto Set = Cast<UK2Node_AssignmentStatement>(Link->GetOwningNode());
			if (!Set || Link != Set->GetVariablePin() || !UE427IsEmpty(Set->GetValuePin()->LinkedTo)) return false;
			const FString Value = Set->GetValuePin()->DefaultValue;
			if (UE427IsEmpty(Value) || Value == TEXT("0") || Value.Equals(TEXT("false"), ESearchCase::IgnoreCase))
			{
				if (FlagInit) return false;
				FlagInit = Set;
			}
			else if (Value == TEXT("1") || Value.Equals(TEXT("true"), ESearchCase::IgnoreCase)) Breaks.Add(Set);
			else return false;
		}
		if (!FlagInit || UE427IsEmpty(Pin(FlagInit, UEdGraphSchema_K2::PN_Execute)->LinkedTo) ||
			!Exclusive(Pin(FlagInit, UEdGraphSchema_K2::PN_Then), Pin(Init, UEdGraphSchema_K2::PN_Execute))) return false;
		Removed.Append({And, Not, Flag->GetOwningNode(), FlagInit});
	}
	TSet<UEdGraphNode*> Body;
	if (!GatherBody(BodyStart, Pin(Increment, UEdGraphSchema_K2::PN_Execute), Removed, Body)) return false;
	for (auto Break : Breaks)
	{
		if (!Body.Contains(Break) || !Linked(Pin(Break, UEdGraphSchema_K2::PN_Then), Pin(Increment, UEdGraphSchema_K2::PN_Execute))) return false;
		Removed.Add(Break);
	}
	TSet<UEdGraphNode*> Seen;
	if (!UsedOnlyInBody(Counter, Removed, Body, Seen) || !UsedOnlyInBody(Index, Removed, Body, Seen)) return false;
	for (auto Element : Elements) if (!UsedOnlyInBody(Element, Removed, Body, Seen)) return false;
	TSet<UEdGraphNode*> Forbidden = Removed;
	Forbidden.Append(Body);
	Seen.Reset();
	if (!IndependentInput(ForEach ? ArrayInput : Init->GetValuePin(), Forbidden, Seen)) return false;
	if (!ForEach && !IndependentInput(LimitInput, Forbidden, Seen)) return false;
	if (ForEach && Counter != Index)
		for (auto Link : Counter->LinkedTo) if (!Removed.Contains(Link->GetOwningNode())) return false;
	auto Library = LoadObject<UBlueprint>(nullptr, TEXT("/Engine/EditorBlueprintResources/StandardMacros.StandardMacros"));
	if (!Library) return false;
	const FName MacroName = Flag ? TEXT("ForEachLoopWithBreak") : Reverse ? TEXT("ReverseForEachLoop") : ForEach ? TEXT("ForEachLoop") : TEXT("ForLoop");
	UEdGraph* Definition = nullptr;
	for (auto MacroGraph : Library->MacroGraphs) if (MacroGraph->GetFName() == MacroName) Definition = MacroGraph;
	if (!Definition) return false;
	auto Macro = NewObject<UK2Node_MacroInstance>(Graph);
	Macro->SetMacroGraph(Definition);
	Macro->CreateNewGuid();
	Macro->NodePosX = Init->NodePosX;
	Macro->NodePosY = Init->NodePosY;
	Graph->AddNode(Macro, false, false);
	Macro->AllocateDefaultPins();
	TArray<TPair<UEdGraphPin*, UEdGraphPin*>> Moves = {
		TPair<UEdGraphPin*, UEdGraphPin*>(Pin(FlagInit ? FlagInit : Init, UEdGraphSchema_K2::PN_Execute), Pin(Macro, ForEach ? TEXT("Exec") : TEXT("execute"))),
		TPair<UEdGraphPin*, UEdGraphPin*>(BodyStart, Pin(Macro, TEXT("LoopBody"))),
		TPair<UEdGraphPin*, UEdGraphPin*>(Branch->GetElsePin(), Pin(Macro, TEXT("Completed"))),
		TPair<UEdGraphPin*, UEdGraphPin*>(Index, Pin(Macro, Reverse ? TEXT("ArrayIndex") : ForEach ? TEXT("Array Index") : TEXT("Index")))
	};
	for (auto Break : Breaks) Moves.Emplace(Pin(Break, UEdGraphSchema_K2::PN_Execute), Pin(Macro, TEXT("Break")));
	if (ForEach)
	{
		Moves.Emplace(ArrayInput, Pin(Macro, TEXT("Array")));
		for (auto Element : Elements) Moves.Emplace(Element, Pin(Macro, Reverse ? TEXT("ArrayElement") : TEXT("Array Element")));
		Macro->ResolvedWildcardType = Source(ArrayInput)->PinType;
		for (auto P : Macro->Pins)
			if (P->PinType.PinCategory == UEdGraphSchema_K2::PC_Wildcard)
			{
				P->PinType.PinCategory = Macro->ResolvedWildcardType.PinCategory;
				P->PinType.PinSubCategory = Macro->ResolvedWildcardType.PinSubCategory;
				P->PinType.PinSubCategoryObject = Macro->ResolvedWildcardType.PinSubCategoryObject;
			}
	}
	else
	{
		Moves.Emplace(Init->GetValuePin(), Pin(Macro, TEXT("FirstIndex")));
		Moves.Emplace(LimitInput, Pin(Macro, TEXT("LastIndex")));
	}
	for (const auto& Move : Moves)
		if (!Move.Key || !Move.Value || Move.Key->PinType.PinCategory != Move.Value->PinType.PinCategory || Move.Key->PinType.ContainerType != Move.Value->PinType.ContainerType)
		{
			Graph->RemoveNode(Macro);
			return false;
		}
	for (const auto& Move : Moves)
		for (auto Link : Move.Key->LinkedTo)
			for (const auto& Other : Moves)
				if (Other.Key == Link)
				{
					Graph->RemoveNode(Macro);
					return false;
				}
	for (const auto& Move : Moves)
	{
		Move.Value->DefaultValue = Move.Key->DefaultValue;
		Move.Value->DefaultObject = Move.Key->DefaultObject;
		Move.Value->DefaultTextValue = Move.Key->DefaultTextValue;
		const auto Links = Move.Key->LinkedTo;
		for (auto Link : Links)
			if (!Removed.Contains(Link->GetOwningNode()))
			{
				if (!(bAggressive && Move.Key == ArrayInput && Length && !Removed.Contains(Length))) Move.Key->BreakLinkTo(Link);
				Move.Value->MakeLinkTo(Link);
			}
			else
				for (const auto& Other : Moves)
					if (Other.Key == Link) Move.Value->MakeLinkTo(Other.Value);
	}
	for (auto Node : Removed) Node->BreakAllNodeLinks();
	for (auto Node : Removed) Graph->RemoveNode(Node);
	UE_LOG(LogCASLoopMacros, Display, TEXT("Recovered %s in %s"), *MacroName.ToString(), *Graph->GetPathName());
	return true;
}

bool RecoverWhile(UEdGraph* Graph, UK2Node_IfThenElse* Branch)
{
	auto Condition = Branch->GetConditionPin();
	TSet<UEdGraphNode*> Seen;
	TFunction<bool(UEdGraphPin*)> ReadsTemporary = [&](UEdGraphPin* Input)
	{
		for (auto Link : Input->LinkedTo)
		{
			auto Node = Link->GetOwningNode();
			if (Cast<UK2Node_TemporaryVariable>(Node)) return true;
			if (Seen.Contains(Node)) continue;
			Seen.Add(Node);
			for (auto P : Node->Pins)
				if (P->Direction == EGPD_Input && P->PinType.PinCategory != UEdGraphSchema_K2::PC_Exec && ReadsTemporary(P)) return true;
		}
		return false;
	};
	if (ReadsTemporary(Condition)) return false;
	auto Execute = Pin(Branch, UEdGraphSchema_K2::PN_Execute);
	auto Start = Pin(Branch, UEdGraphSchema_K2::PN_Then);
	TSet<UEdGraphNode*> Body;
	if (Execute->LinkedTo.Num() < 2 || !GatherBody(Start, Execute, {Branch}, Body, true) || UE427IsEmpty(Body)) return false;
	TArray<UEdGraphPin*> Entries;
	bool bHasBackEdge = false;
	for (auto Link : Execute->LinkedTo)
		if (Body.Contains(Link->GetOwningNode())) bHasBackEdge = true;
		else Entries.Add(Link);
	if (!bHasBackEdge || UE427IsEmpty(Entries) || Entries.Contains(Branch->GetElsePin())) return false;
	auto Library = LoadObject<UBlueprint>(nullptr, TEXT("/Engine/EditorBlueprintResources/StandardMacros.StandardMacros"));
	if (!Library) return false;
	UEdGraph* Definition = nullptr;
	for (auto G : Library->MacroGraphs) if (G->GetFName() == TEXT("WhileLoop")) Definition = G;
	if (!Definition) return false;
	auto Macro = NewObject<UK2Node_MacroInstance>(Graph);
	Macro->SetMacroGraph(Definition);
	Macro->CreateNewGuid();
	Macro->NodePosX = Branch->NodePosX;
	Macro->NodePosY = Branch->NodePosY;
	Graph->AddNode(Macro, false, false);
	Macro->AllocateDefaultPins();
	if (!Pin(Macro, TEXT("execute")) || !Pin(Macro, TEXT("Condition")) || !Pin(Macro, TEXT("LoopBody")) || !Pin(Macro, TEXT("Completed")))
	{
		Graph->RemoveNode(Macro);
		return false;
	}
	for (auto Entry : Entries) Entry->MakeLinkTo(Pin(Macro, TEXT("execute")));
	Pin(Macro, TEXT("Condition"))->DefaultValue = Condition->DefaultValue;
	for (auto Input : Condition->LinkedTo) Input->MakeLinkTo(Pin(Macro, TEXT("Condition")));
	for (auto Input : Start->LinkedTo) Pin(Macro, TEXT("LoopBody"))->MakeLinkTo(Input);
	for (auto Input : Branch->GetElsePin()->LinkedTo) Pin(Macro, TEXT("Completed"))->MakeLinkTo(Input);
	Branch->BreakAllNodeLinks();
	Graph->RemoveNode(Branch);
	UE_LOG(LogCASLoopMacros, Display, TEXT("Recovered WhileLoop in %s"), *Graph->GetPathName());
	return true;
}
}

int32 FCASMacroReconstruction::ApplyLoops(UEdGraph* Graph, bool bAggressive)
{
	if (!Graph) return 0;
    if (bAggressive)
    {
        const auto Reroutes = Graph->Nodes;
        for (auto Node : Reroutes)
            if (auto Knot = Cast<UK2Node_Knot>(Node))
            {
                auto Input = Knot->GetInputPin();
                auto Output = Knot->GetOutputPin();
                if (Input->LinkedTo.Num() != 1 || Input->LinkedTo[0]->GetOwningNode() == Knot || Output->LinkedTo.Contains(Input)) continue;
                auto Upstream = Input->LinkedTo[0];
                const auto Targets = Output->LinkedTo;
                Knot->BreakAllNodeLinks();
                for (auto Target : Targets) Upstream->MakeLinkTo(Target);
                Graph->RemoveNode(Knot);
            }
    }
    int32 Count = 0;
    const auto Nodes = Graph->Nodes;
	for (auto Node : Nodes)
		if (Graph->Nodes.Contains(Node))
			if (auto Branch = Cast<UK2Node_IfThenElse>(Node))
				if (RecoverLoop(Graph, Branch, bAggressive)) ++Count;
	const auto Remaining = Graph->Nodes;
	for (auto Node : Remaining)
		if (Graph->Nodes.Contains(Node))
			if (auto Branch = Cast<UK2Node_IfThenElse>(Node))
				if (RecoverWhile(Graph, Branch)) ++Count;
	return Count;
}
