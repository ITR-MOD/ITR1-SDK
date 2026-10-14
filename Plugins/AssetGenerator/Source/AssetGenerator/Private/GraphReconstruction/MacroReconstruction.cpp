#include "MacroReconstruction.h"
#include "UE427AssetPath.h"
#include "EdGraph/EdGraph.h"
#include "EdGraphSchema_K2.h"
#include "Engine/Blueprint.h"
#include "K2Node_CallFunction.h"
#include "K2Node_IfThenElse.h"
#include "K2Node_MacroInstance.h"
#include "Kismet/KismetSystemLibrary.h"

int32 FCASMacroReconstruction::Apply(UEdGraph* Graph)
{
	if (!Graph) return 0;
	int32 Count = 0;
	UEdGraph* Definition = nullptr;
	const auto Nodes = Graph->Nodes;
	for (UEdGraphNode* Node : Nodes)
	{
		auto Branch = Cast<UK2Node_IfThenElse>(Node);
		if (!Branch) continue;
		auto Condition = Branch->GetConditionPin();
		if (!Condition || Condition->LinkedTo.Num() != 1) continue;
		auto Result = Condition->LinkedTo[0];
		auto Call = Cast<UK2Node_CallFunction>(Result->GetOwningNode());
		if (!Call || Call->GetTargetFunction() != UKismetSystemLibrary::StaticClass()->FindFunctionByName(TEXT("IsValid")) ||
			Result != Call->FindPin(UEdGraphSchema_K2::PN_ReturnValue) || Result->LinkedTo.Num() != 1) continue;
		auto Object = Call->FindPin(TEXT("Object"), EGPD_Input);
		auto Execute = Branch->FindPin(UEdGraphSchema_K2::PN_Execute, EGPD_Input);
		auto Then = Branch->FindPin(UEdGraphSchema_K2::PN_Then, EGPD_Output);
		auto Else = Branch->GetElsePin();
		if (!Object || !Execute || !Then || !Else || Object->LinkedTo.Num() > 1 || UE427IsEmpty(Execute->LinkedTo)) continue;
		bool ExtraLinks = false;
		for (auto Pin : Call->Pins)
			if (Pin != Object && Pin != Result && !UE427IsEmpty(Pin->LinkedTo)) ExtraLinks = true;
		if (ExtraLinks) continue;
		if (!Definition)
		{
			auto Library = LoadObject<UBlueprint>(nullptr, TEXT("/Engine/EditorBlueprintResources/StandardMacros.StandardMacros"));
			if (!Library) return Count;
			for (UEdGraph* Macro : Library->MacroGraphs)
				if (Macro && Macro->GetFName() == TEXT("IsValid")) Definition = Macro;
			if (!Definition) return Count;
		}
		auto Macro = NewObject<UK2Node_MacroInstance>(Graph);
		Macro->SetMacroGraph(Definition);
		Macro->CreateNewGuid();
		Macro->NodePosX = Branch->NodePosX;
		Macro->NodePosY = Branch->NodePosY;
		Graph->AddNode(Macro, false, false);
		Macro->AllocateDefaultPins();
		TArray<TPair<UEdGraphPin*, UEdGraphPin*>> Pins = {
			TPair<UEdGraphPin*, UEdGraphPin*>(Execute, Macro->FindPin(TEXT("exec"), EGPD_Input)),
			TPair<UEdGraphPin*, UEdGraphPin*>(Object, Macro->FindPin(TEXT("InputObject"), EGPD_Input)),
			TPair<UEdGraphPin*, UEdGraphPin*>(Then, Macro->FindPin(TEXT("Is Valid"), EGPD_Output)),
			TPair<UEdGraphPin*, UEdGraphPin*>(Else, Macro->FindPin(TEXT("Is Not Valid"), EGPD_Output))
		};
		bool Compatible = true;
		for (const auto& Pair : Pins)
		{
			if (!Pair.Value) { Compatible = false; continue; }
			auto SourceType = Pair.Key->PinType;
			auto TargetType = Pair.Value->PinType;
			SourceType.bIsConst = TargetType.bIsConst = false;
			if (SourceType != TargetType) Compatible = false;
		}
		if (!Compatible)
		{
			Graph->RemoveNode(Macro);
			continue;
		}
		for (const auto& Pair : Pins)
		{
			Pair.Value->DefaultValue = Pair.Key->DefaultValue;
			Pair.Value->DefaultObject = Pair.Key->DefaultObject;
			Pair.Value->DefaultTextValue = Pair.Key->DefaultTextValue;
			if (Pair.Key == Object && !Pair.Value->DefaultObject && Pair.Value->DefaultValue == TEXT("None")) Pair.Value->DefaultValue.Reset();
			const auto Links = Pair.Key->LinkedTo;
			Pair.Key->BreakAllPinLinks();
			for (auto Link : Links) Pair.Value->MakeLinkTo(Link);
		}
		Branch->BreakAllNodeLinks();
		Call->BreakAllNodeLinks();
		Graph->RemoveNode(Branch);
		Graph->RemoveNode(Call);
		++Count;
	}
	return Count;
}
