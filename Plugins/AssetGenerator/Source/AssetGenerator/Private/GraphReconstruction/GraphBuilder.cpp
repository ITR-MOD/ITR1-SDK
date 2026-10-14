#include "GraphBuilder.h"
#include "UE427AssetPath.h"

#include "UObject/Script.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/SimpleConstructionScript.h"
#include "Engine/SCS_Node.h"
#include "Components/ActorComponent.h"
#include "Engine/TimelineTemplate.h"
#include "Components/TimelineComponent.h"
#include "Curves/CurveFloat.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "EdGraphSchema_K2.h"
#include "K2Node_CallFunction.h"
#include "K2Node_CallParentFunction.h"
#include "K2Node_CallArrayFunction.h"
#include "K2Node_Event.h"
#include "K2Node_CustomEvent.h"
#include "K2Node_FunctionEntry.h"
#include "K2Node_FunctionResult.h"
#include "K2Node_IfThenElse.h"
#include "K2Node_VariableGet.h"
#include "K2Node_VariableSet.h"
#include "K2Node_DynamicCast.h"
#include "K2Node_ClassDynamicCast.h"
#include "K2Node_Self.h"
#include "K2Node_FunctionTerminator.h"
#include "K2Node_ExecutionSequence.h"
#include "K2Node_TemporaryVariable.h"
#include "K2Node_AssignmentStatement.h"
#include "K2Node_MakeArray.h"
#include "K2Node_MakeStruct.h"
#include "K2Node_SetFieldsInStruct.h"
#include "K2Node_GetClassDefaults.h"
#include "K2Node_Select.h"
#include "K2Node_InputAction.h"
#include "K2Node_InputAxisEvent.h"
#include "K2Node_InputAxisKeyEvent.h"
#include "K2Node_InputKey.h"
#include "K2Node_Timeline.h"
#include "K2Node_MakeMap.h"
#include "K2Node_CreateDelegate.h"
#include "K2Node_AddDelegate.h"
#include "K2Node_RemoveDelegate.h"
#include "K2Node_ClearDelegate.h"
#include "K2Node_CallDelegate.h"
#include "K2Node_ComponentBoundEvent.h"
#include "K2Node_BreakStruct.h"
#include "K2Node_GetArrayItem.h"
#include "UObject/UnrealType.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "EdGraph/EdGraphPin.h"
#include "BlueprintNodeSpawner.h"

#define LOCTEXT_NAMESPACE "BlueprintUncooker"

DEFINE_LOG_CATEGORY_STATIC(LogGraphBuilder, Log, All);

FCASGraphBuilder::FCASGraphBuilder()
{
}

void FCASGraphBuilder::BuildGraphs(UBlueprint* Blueprint, UClass* SourceClass, const TArray<FCASDecompiledFunction>& Functions)
{
	CurrentOriginalClass = SourceClass;
	EventStubParamNames.Reset();
	TArray<FCASDecompiledFunction> Stubs;
	const FCASDecompiledFunction* Ubergraph = nullptr;
	for (const FCASDecompiledFunction& Function : Functions)
	{
		if (Function.bIsUbergraph) Ubergraph = &Function;
		if (Function.bIsEventStub)
		{
			Stubs.Add(Function);
			for (TFieldIterator<FProperty> It(Function.OriginalFunction); It; ++It)
				if (It->HasAnyPropertyFlags(CPF_Parm) && !It->HasAnyPropertyFlags(CPF_ReturnParm)) EventStubParamNames.Add(It->GetFName());
		}
	}
	if (Ubergraph)
	{
		UEdGraph* EventGraph = FBlueprintEditorUtils::FindEventGraph(Blueprint);
		if (!EventGraph)
		{
			EventGraph = FBlueprintEditorUtils::CreateNewGraph(Blueprint, UEdGraphSchema_K2::GN_EventGraph, UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());
			Blueprint->UbergraphPages.Add(EventGraph);
		}
		for (int32 Index = EventGraph->Nodes.Num() - 1; Index >= 0; --Index)
			EventGraph->RemoveNode(EventGraph->Nodes[Index]);
		BuildEventGraph(Blueprint, EventGraph, *Ubergraph, Stubs, SourceClass);
	}
	for (const FCASDecompiledFunction& Function : Functions)
	{
		if (Function.bIsUbergraph || Function.bIsEventStub || UE427IsEmpty(Function.Statements)) continue;
		int32 InterfaceIndex = INDEX_NONE;
		for (int32 Index = 0; Index < Blueprint->ImplementedInterfaces.Num(); ++Index)
		{
			auto& Interface = Blueprint->ImplementedInterfaces[Index];
			if (!Interface.Interface || !Interface.Interface->FindFunctionByName(*Function.FunctionName)) continue;
			InterfaceIndex = Index;
			for (int32 GraphIndex = Interface.Graphs.Num() - 1; GraphIndex >= 0; --GraphIndex)
				if (Interface.Graphs[GraphIndex] && Interface.Graphs[GraphIndex]->GetName() == Function.FunctionName)
				{
					UEdGraph* PreviousGraph = Interface.Graphs[GraphIndex];
					Interface.Graphs.RemoveAt(GraphIndex);
					FBlueprintEditorUtils::RemoveGraph(Blueprint, PreviousGraph, EGraphRemoveFlags::MarkTransient);
				}
			break;
		}
		BuildFunctionGraph(Blueprint, Function, SourceClass);
		if (InterfaceIndex != INDEX_NONE)
			for (int32 Index = Blueprint->FunctionGraphs.Num() - 1; Index >= 0; --Index)
				if (Blueprint->FunctionGraphs[Index]->GetName() == Function.FunctionName)
				{
					Blueprint->ImplementedInterfaces[InterfaceIndex].Graphs.Add(Blueprint->FunctionGraphs[Index]);
					Blueprint->FunctionGraphs.RemoveAt(Index);
					break;
				}
	}
}

void FCASGraphBuilder::Warn(const FString& Message)
{
	Warnings.Add(Message);
	UE_LOG(LogGraphBuilder, Warning, TEXT("%s"), *Message);
}

static void ResolveWildcardAfterConnect(UEdGraphPin* A, UEdGraphPin* B)
{
	if (!A || !B) return;

	UEdGraphPin* Output = (A->Direction == EGPD_Output) ? A : B;
	UEdGraphPin* Input = (A->Direction == EGPD_Output) ? B : A;

	bool bOutputWild = Output->PinType.PinCategory == UEdGraphSchema_K2::PC_Wildcard;
	bool bInputWild = Input->PinType.PinCategory == UEdGraphSchema_K2::PC_Wildcard;

	if (bOutputWild && !bInputWild)
	{

		EPinContainerType OrigContainer = Output->PinType.ContainerType;
		Output->PinType = Input->PinType;
		if (OrigContainer == EPinContainerType::Array && Input->PinType.ContainerType != EPinContainerType::Array)
		{
			Output->PinType.ContainerType = OrigContainer;
		}
	}
	else if (bInputWild && !bOutputWild)
	{
		EPinContainerType OrigContainer = Input->PinType.ContainerType;
		Input->PinType = Output->PinType;
		if (OrigContainer == EPinContainerType::Array && Output->PinType.ContainerType != EPinContainerType::Array)
		{
			Input->PinType.ContainerType = OrigContainer;
		}
	}
}

bool FCASGraphBuilder::TryConnect(UEdGraphPin* A, UEdGraphPin* B)
{
	if (!A || !B) return false;

	if (A->GetOwningNode() == B->GetOwningNode()) return false;

	const UEdGraphSchema_K2* Schema = GetDefault<UEdGraphSchema_K2>();
	if (Schema->TryCreateConnection(A, B))
	{
		ResolveWildcardAfterConnect(A, B);
		return true;
	}

	if (Schema->TryCreateConnection(B, A))
	{
		ResolveWildcardAfterConnect(B, A);
		return true;
	}

	return false;
}

bool FCASGraphBuilder::ForceConnect(UEdGraphPin* A, UEdGraphPin* B)
{
	if (!A || !B) return false;
	if (A->GetOwningNode() == B->GetOwningNode()) return false;

	UEdGraphPin* Output = (A->Direction == EGPD_Output) ? A : B;
	UEdGraphPin* Input = (A->Direction == EGPD_Output) ? B : A;
	if (Output->Direction != EGPD_Output || Input->Direction != EGPD_Input) return false;
	if (Output->PinType.PinCategory == UEdGraphSchema_K2::PC_Object && Input->PinType.PinCategory == UEdGraphSchema_K2::PC_Object &&
		!Output->PinType.IsContainer() && !Input->PinType.IsContainer())
	{
		auto SourceClass = Cast<UClass>(Output->PinType.PinSubCategoryObject.Get());
		auto TargetClass = Cast<UClass>(Input->PinType.PinSubCategoryObject.Get());
		if (SourceClass && TargetClass && SourceClass != TargetClass && TargetClass->IsChildOf(SourceClass))
		{
			auto Graph = Input->GetOwningNode()->GetGraph();
			auto CastNode = NewObject<UK2Node_DynamicCast>(Graph);
			CastNode->CreateNewGuid();
			CastNode->TargetType = TargetClass;
			CastNode->SetPurity(true);
			CastNode->NodePosX = Input->GetOwningNode()->NodePosX - 200;
			CastNode->NodePosY = Input->GetOwningNode()->NodePosY - 100;
			Graph->AddNode(CastNode, false, false);
			CastNode->AllocateDefaultPins();
			auto Source = CastNode->GetCastSourcePin();
			Source->PinType = Output->PinType;
			Output->MakeLinkTo(Source);
			CastNode->PinConnectionListChanged(Source);
			CastNode->GetCastResultPin()->MakeLinkTo(Input);
			return true;
		}
	}

	bool bOutputWild = Output->PinType.PinCategory == UEdGraphSchema_K2::PC_Wildcard;
	bool bInputWild = Input->PinType.PinCategory == UEdGraphSchema_K2::PC_Wildcard;

	if (bOutputWild && !bInputWild)
	{
		Output->PinType = Input->PinType;
		Output->MakeLinkTo(Input);

		Output->GetOwningNode()->PinConnectionListChanged(Output);
		return true;
	}
	else if (bInputWild && !bOutputWild)
	{
		Input->PinType = Output->PinType;
		Output->MakeLinkTo(Input);
		Input->GetOwningNode()->PinConnectionListChanged(Input);
		return true;
	}

	if (auto Assignment = Cast<UK2Node_AssignmentStatement>(Input->GetOwningNode()); Assignment && Input == Assignment->GetValuePin() &&
		Output->PinType.PinCategory == UEdGraphSchema_K2::PC_Object && Input->PinType.PinCategory == UEdGraphSchema_K2::PC_Object &&
		!Output->PinType.IsContainer() && !Input->PinType.IsContainer())
	{
		auto SourceClass = Cast<UClass>(Output->PinType.PinSubCategoryObject.Get());
		auto TargetClass = Cast<UClass>(Input->PinType.PinSubCategoryObject.Get());
		if (SourceClass && TargetClass && SourceClass != TargetClass && SourceClass->IsChildOf(TargetClass))
		{
			auto Graph = Input->GetOwningNode()->GetGraph();
			auto CastNode = NewObject<UK2Node_DynamicCast>(Graph);
			CastNode->CreateNewGuid();
			CastNode->TargetType = TargetClass;
			CastNode->SetPurity(true);
			CastNode->NodePosX = Assignment->NodePosX - 200;
			CastNode->NodePosY = Assignment->NodePosY - 150;
			Graph->AddNode(CastNode, false, false);
			CastNode->AllocateDefaultPins();
			Output->MakeLinkTo(CastNode->GetCastSourcePin());
			CastNode->NotifyPinConnectionListChanged(CastNode->GetCastSourcePin());
			CastNode->GetCastResultPin()->MakeLinkTo(Input);
			return true;
		}
	}
	if (TryConnect(A, B)) return true;

	if (Output->PinType.PinCategory == UEdGraphSchema_K2::PC_Object &&
		Input->PinType.PinCategory == UEdGraphSchema_K2::PC_Object)
	{
		UClass* OutClass = Cast<UClass>(Output->PinType.PinSubCategoryObject.Get());
		UClass* InClass = Cast<UClass>(Input->PinType.PinSubCategoryObject.Get());

		if (!OutClass || !InClass)
		{
			Output->MakeLinkTo(Input);
			return true;
		}

		if (OutClass == InClass ||
			OutClass->GetFName() == InClass->GetFName())
		{
			Output->MakeLinkTo(Input);
			return true;
		}

		if (OutClass->IsChildOf(InClass))
		{
			Output->MakeLinkTo(Input);
			return true;
		}

		if (InClass->IsChildOf(OutClass) && !Output->PinType.IsContainer() && !Input->PinType.IsContainer())
		{
			UClass* CastTarget = InClass;
			UEdGraph* Graph = Output->GetOwningNode()->GetGraph();
			if (Graph)
			{
				UK2Node_DynamicCast* CastNode = NewObject<UK2Node_DynamicCast>(Graph);
				CastNode->CreateNewGuid();
				CastNode->TargetType = CastTarget;
				CastNode->SetPurity(true);
				CastNode->NodePosX = (Output->GetOwningNode()->NodePosX + Input->GetOwningNode()->NodePosX) / 2;
				CastNode->NodePosY = Output->GetOwningNode()->NodePosY - 50;
				CastNode->AllocateDefaultPins();
				Graph->AddNode(CastNode, false, false);

				UEdGraphPin* CastObjectPin = CastNode->GetCastSourcePin();
				if (CastObjectPin)
				{
					Output->MakeLinkTo(CastObjectPin);
					CastNode->NotifyPinConnectionListChanged(CastObjectPin);
				}

				UEdGraphPin* CastResultPin = CastNode->GetCastResultPin();
				if (CastResultPin)
				{
					CastResultPin->MakeLinkTo(Input);
					return true;
				}
			}
		}

		Output->MakeLinkTo(Input);
		return true;
	}

	if ((Output->PinType.PinCategory == UEdGraphSchema_K2::PC_Object &&
		Input->PinType.PinCategory == UEdGraphSchema_K2::PC_Interface) ||
		(Output->PinType.PinCategory == UEdGraphSchema_K2::PC_Interface &&
			Input->PinType.PinCategory == UEdGraphSchema_K2::PC_Object))
	{
		UClass* TargetClass = Cast<UClass>(Input->PinType.PinSubCategoryObject.Get());
		if (TargetClass)
		{
			UEdGraph* Graph = Output->GetOwningNode()->GetGraph();
			if (Graph)
			{
				UK2Node_DynamicCast* CastNode = NewObject<UK2Node_DynamicCast>(Graph);
				CastNode->CreateNewGuid();
				CastNode->TargetType = TargetClass;
				CastNode->SetPurity(true);
				CastNode->NodePosX = (Output->GetOwningNode()->NodePosX + Input->GetOwningNode()->NodePosX) / 2;
				CastNode->NodePosY = Output->GetOwningNode()->NodePosY - 50;
				CastNode->AllocateDefaultPins();
				Graph->AddNode(CastNode, false, false);

				UEdGraphPin* CastObjectPin = CastNode->GetCastSourcePin();
				if (CastObjectPin) {
					Output->MakeLinkTo(CastObjectPin);
					CastNode->NotifyPinConnectionListChanged(CastObjectPin);
				}

				UEdGraphPin* CastResultPin = CastNode->GetCastResultPin();
				if (CastResultPin)
				{
					CastResultPin->MakeLinkTo(Input);
					return true;
				}
			}
		}
	}

	Output->MakeLinkTo(Input);
	return true;
}

void FCASGraphBuilder::SanitizeGraphConnections(UEdGraph* Graph)
{
	if (!Graph) return;

	int32 BrokenCount = 0;

	for (UEdGraphNode* Node : Graph->Nodes)
	{
		if (!Node) continue;

		for (UEdGraphPin* Pin : Node->Pins)
		{
			if (!Pin) continue;

			for (int32 i = Pin->LinkedTo.Num() - 1; i >= 0; --i)
			{
				UEdGraphPin* LinkedPin = Pin->LinkedTo[i];
				if (!LinkedPin) continue;

				if (Pin->Direction == LinkedPin->Direction)
				{

					Pin->LinkedTo.RemoveAt(i);

					LinkedPin->LinkedTo.Remove(Pin);

					BrokenCount++;

					UE_LOG(LogGraphBuilder, Warning,
						TEXT("Sanitize: removed direction-mismatched link between '%s' (%s) and '%s' (%s)"),
						*Pin->GetDisplayName().ToString(),
						*Node->GetNodeTitle(ENodeTitleType::ListView).ToString(),
						*LinkedPin->GetDisplayName().ToString(),
						*LinkedPin->GetOwningNode()->GetNodeTitle(ENodeTitleType::ListView).ToString());
				}
			}
		}
	}

	if (BrokenCount > 0)
	{
		UE_LOG(LogGraphBuilder, Warning,
			TEXT("Sanitize: removed %d direction-mismatched connection(s) from graph '%s'"),
			BrokenCount, *Graph->GetName());
	}
}

bool FCASGraphBuilder::ApplyTempLiteralToPin(TSharedPtr<FCASDecompiledExpr> Expr, UEdGraphPin* Pin)
{
	if (!Expr || !Pin) return false;

	FName VarName = NAME_None;
	if ((Expr->Token == EX_LocalVariable || Expr->Token == EX_InstanceVariable ||
		Expr->Token == EX_LocalOutVariable || Expr->Token == EX_DefaultVariable) &&
		Expr->PropertyRef)
	{
		VarName = Expr->PropertyRef->GetFName();
	}

	if (VarName == NAME_None && !Expr->IsLiteral()) return false;

	if (MultiAssignedTempVars.Contains(VarName)) return false;

	TSharedPtr<FCASDecompiledExpr>* StoredLiteral = TempLiteralMap.Find(VarName);
	if ((!StoredLiteral || !(*StoredLiteral)) && !Expr->IsLiteral()) return false;

	TSharedPtr<FCASDecompiledExpr> LitExpr = Expr->IsLiteral() ? Expr : *StoredLiteral;
	if ((LitExpr->Token == EX_StructConst || LitExpr->Token == EX_VectorConst) && Pin->PinType.bIsReference)
	{
		auto Node = Pin->GetOwningNode();
		int32 LiteralX = Node->NodePosX - NodeSpacingX;
		int32 LiteralY = Node->NodePosY;
		auto Output = ResolveDataExpr(Node->GetGraph(), LitExpr, LiteralX, LiteralY);
		if (!Output) return false;
		ForceConnect(Output, Pin);
		return Pin->LinkedTo.Contains(Output);
	}

	switch (LitExpr->Token)
	{
	case EX_ObjectConst:
		if (LitExpr->ObjectRef)
		{
			Pin->DefaultObject = LitExpr->ObjectRef;
			Pin->GetOwningNode()->PinDefaultValueChanged(Pin);
		}
		return true;

	case EX_NoObject:
		Pin->DefaultValue = TEXT("");
		return true;

	case EX_Self:

		return true;

	case EX_ByteConst:
	case EX_IntConst:
	case EX_IntConstByte:
	case EX_IntZero:
	case EX_IntOne:
	{
		int64 EnumVal = 0;
		if (LitExpr->Token == EX_ByteConst)
			EnumVal = LitExpr->ByteValue;
		else
			EnumVal = LitExpr->IntValue;

		if (Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Byte &&
			Pin->PinType.PinSubCategoryObject.IsValid())
		{
			UEnum* PinEnum = Cast<UEnum>(Pin->PinType.PinSubCategoryObject.Get());
			if (PinEnum)
			{
				FString EnumName = PinEnum->GetNameStringByValue(EnumVal);
				if (!UE427IsEmpty(EnumName))
				{
					Pin->DefaultValue = EnumName;
					return true;
				}
			}
		}
		Pin->DefaultValue = LitExpr->GetLiteralAsString();
		return true;
	}

	default:
	{
		FString DefaultVal = LitExpr->GetLiteralAsString();
		if (!UE427IsEmpty(DefaultVal))
		{
			Pin->DefaultValue = DefaultVal;
		}
		return true;
	}
	}
}

UEdGraphPin* FCASGraphBuilder::FindExecPin(UEdGraphNode* Node, EEdGraphPinDirection Direction, FName PinName)
{
	if (!Node) return nullptr;

	for (UEdGraphPin* Pin : Node->Pins)
	{
		if (Pin->Direction == Direction && Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Exec)
		{
			if (PinName == NAME_None || Pin->PinName == PinName)
			{
				return Pin;
			}
		}
	}

	if (PinName != NAME_None)
	{
		FString TargetStr = PinName.ToString().Replace(TEXT(" "), TEXT(""));
		for (UEdGraphPin* Pin : Node->Pins)
		{
			if (Pin->Direction == Direction && Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Exec)
			{
				FString PinStr = Pin->PinName.ToString().Replace(TEXT(" "), TEXT(""));
				if (PinStr.Equals(TargetStr, ESearchCase::IgnoreCase))
				{
					return Pin;
				}
			}
		}
	}

	return nullptr;
}

UEdGraphPin* FCASGraphBuilder::FindDataPin(UEdGraphNode* Node, EEdGraphPinDirection Direction, FName PinName)
{
	if (!Node) return nullptr;

	for (UEdGraphPin* Pin : Node->Pins)
	{
		if (Pin->Direction == Direction && Pin->PinName == PinName &&
			Pin->PinType.PinCategory != UEdGraphSchema_K2::PC_Exec)
		{
			return Pin;
		}
	}
	return nullptr;
}

UEdGraphPin* FCASGraphBuilder::FindFirstDataOutputPin(UEdGraphNode* Node)
{
	if (!Node) return nullptr;
	if (auto Call = Cast<UK2Node_CallFunction>(Node))
	{
		if (const UFunction* Function = Call->GetTargetFunction())
			if (const FProperty* Return = Function->GetReturnProperty())
				if (UEdGraphPin* Pin = Call->FindPin(Return->GetFName(), EGPD_Output)) return Pin;
		if (UEdGraphPin* Pin = Call->FindPin(UEdGraphSchema_K2::PN_ReturnValue, EGPD_Output)) return Pin;
	}

	for (UEdGraphPin* Pin : Node->Pins)
	{
		if (Pin->Direction == EGPD_Output && Pin->PinType.PinCategory != UEdGraphSchema_K2::PC_Exec)
		{
			return Pin;
		}
	}
	return nullptr;
}

FName FCASGraphBuilder::GetPinNameForFunctionParam(UFunction* Function, int32 ParamIndex)
{
	if (!Function) return NAME_None;

	int32 Index = 0;
	for (TFieldIterator<FProperty> It(Function); It; ++It)
	{
		FProperty* Prop = *It;

		if (!Prop->HasAnyPropertyFlags(CPF_Parm) || Prop->HasAnyPropertyFlags(CPF_ReturnParm))
			continue;

		if (Index == ParamIndex)
		{
			return Prop->GetFName();
		}
		Index++;
	}
	return NAME_None;
}

UBlueprint* FCASGraphBuilder::BuildBlueprint(
	UClass* OriginalClass,
	const TArray<FCASDecompiledFunction>& Functions,
	UPackage* TargetPackage,
	const FString& BlueprintName)
{
	if (!OriginalClass || !TargetPackage)
	{
		Warn(TEXT("Null class or package"));
		return nullptr;
	}

	UClass* ParentClass = OriginalClass->GetSuperClass();
	if (!ParentClass)
	{
		ParentClass = AActor::StaticClass();
		Warn(TEXT("Could not determine parent class, defaulting to AActor"));
	}

	UE_LOG(LogGraphBuilder, Log, TEXT("Creating Blueprint '%s' with parent '%s'"),
		*BlueprintName, *ParentClass->GetName());

	UBlueprint* NewBP = FKismetEditorUtilities::CreateBlueprint(
		ParentClass,
		TargetPackage,
		FName(*BlueprintName),
		BPTYPE_Normal,
		UBlueprint::StaticClass(),
		UBlueprintGeneratedClass::StaticClass());

	if (!NewBP)
	{
		Warn(TEXT("FKismetEditorUtilities::CreateBlueprint returned null"));
		return nullptr;
	}

	if (!BuildBlueprintCore(NewBP, OriginalClass, Functions))
	{
		return nullptr;
	}

	for (FBPInterfaceDescription& InterfaceDesc : NewBP->ImplementedInterfaces)
	{
		if (!InterfaceDesc.Interface) continue;

		for (TFieldIterator<UFunction> FuncIt(InterfaceDesc.Interface); FuncIt; ++FuncIt)
		{
			FName FuncName = FuncIt->GetFName();

			for (int32 i = NewBP->FunctionGraphs.Num() - 1; i >= 0; --i)
			{
				if (NewBP->FunctionGraphs[i]->GetFName() == FuncName)
				{
					InterfaceDesc.Graphs.Add(NewBP->FunctionGraphs[i]);
					NewBP->FunctionGraphs.RemoveAt(i);
					UE_LOG(LogGraphBuilder, Log, TEXT("Moved interface function '%s' to InterfaceDesc.Graphs"),
						*FuncName.ToString());
					break;
				}
			}
		}
	}

	return NewBP;
}

UBlueprint* FCASGraphBuilder::LiveBuildBlueprint(
	UClass* OriginalClass,
	const TArray<FCASDecompiledFunction>& Functions,
	UBlueprintGeneratedClass* ExistingBPGC)
{
	if (!OriginalClass || !ExistingBPGC)
	{
		Warn(TEXT("Null class or BPGC for LiveBuildBlueprint"));
		return nullptr;
	}

	UClass* ParentClass = OriginalClass->GetSuperClass();
	if (!ParentClass)
	{
		ParentClass = AActor::StaticClass();
		Warn(TEXT("Could not determine parent class, defaulting to AActor"));
	}

	FString BPName = ExistingBPGC->GetName();
	if (BPName.EndsWith(TEXT("_C")))
	{
		BPName.LeftChopInline(2);
	}

	UE_LOG(LogGraphBuilder, Log, TEXT("Live uncooking '%s' — modifying existing BPGC in place"),
		*BPName);

	UPackage* TransientPkg = GetTransientPackage();
	UBlueprint* BP = NewObject<UBlueprint>(TransientPkg, FName(*BPName));
	BP->ParentClass = ParentClass;
	BP->GeneratedClass = ExistingBPGC;
	BP->SkeletonGeneratedClass = ExistingBPGC;
	BP->BlueprintType = BPTYPE_Normal;
	ExistingBPGC->ClassGeneratedBy = BP;

	UEdGraph* EventGraph = FBlueprintEditorUtils::CreateNewGraph(BP,
		UEdGraphSchema_K2::GN_EventGraph,
		UEdGraph::StaticClass(),
		UEdGraphSchema_K2::StaticClass());
	BP->UbergraphPages.Add(EventGraph);

	if (!BuildBlueprintCore(BP, OriginalClass, Functions,  true))
	{
		return nullptr;
	}

	for (FBPInterfaceDescription& InterfaceDesc : BP->ImplementedInterfaces)
	{
		if (!InterfaceDesc.Interface) continue;

		for (TFieldIterator<UFunction> FuncIt(InterfaceDesc.Interface); FuncIt; ++FuncIt)
		{
			FName FuncName = FuncIt->GetFName();

			for (int32 i = BP->FunctionGraphs.Num() - 1; i >= 0; --i)
			{
				if (BP->FunctionGraphs[i]->GetFName() == FuncName)
				{
					InterfaceDesc.Graphs.Add(BP->FunctionGraphs[i]);
					BP->FunctionGraphs.RemoveAt(i);
					break;
				}
			}
		}
	}

	return BP;
}

UBlueprint* FCASGraphBuilder::BuildChildBlueprint(
	UClass* OriginalClass,
	const TArray<FCASDecompiledFunction>& Functions,
	UPackage* TargetPackage,
	const FString& BlueprintName)
{
	if (!OriginalClass || !TargetPackage)
	{
		Warn(TEXT("Null class or package for BuildChildBlueprint"));
		return nullptr;
	}

	UE_LOG(LogGraphBuilder, Log, TEXT("Creating child Blueprint '%s' inheriting from '%s'"),
		*BlueprintName, *OriginalClass->GetName());

	UBlueprint* NewBP = FKismetEditorUtilities::CreateBlueprint(
		OriginalClass,  
		TargetPackage,
		FName(*BlueprintName),
		BPTYPE_Normal,
		UBlueprint::StaticClass(),
		UBlueprintGeneratedClass::StaticClass());

	if (!NewBP)
	{
		Warn(TEXT("FKismetEditorUtilities::CreateBlueprint returned null for child"));
		return nullptr;
	}

	bBuildingChildClass = true;

	if (!BuildBlueprintCore(NewBP, OriginalClass, Functions,  false,  true))
	{
		bBuildingChildClass = false;
		return nullptr;
	}
	bBuildingChildClass = false;

	for (FBPInterfaceDescription& InterfaceDesc : NewBP->ImplementedInterfaces)
	{
		if (!InterfaceDesc.Interface) continue;

		for (TFieldIterator<UFunction> FuncIt(InterfaceDesc.Interface); FuncIt; ++FuncIt)
		{
			FName FuncName = FuncIt->GetFName();

			for (int32 i = NewBP->FunctionGraphs.Num() - 1; i >= 0; --i)
			{
				if (NewBP->FunctionGraphs[i]->GetFName() == FuncName)
				{
					InterfaceDesc.Graphs.Add(NewBP->FunctionGraphs[i]);
					NewBP->FunctionGraphs.RemoveAt(i);
					break;
				}
			}
		}
	}

	return NewBP;
}

bool FCASGraphBuilder::BuildBlueprintCore(
	UBlueprint* NewBP,
	UClass* OriginalClass,
	const TArray<FCASDecompiledFunction>& Functions,
	bool bIsLiveUncook,
	bool bIsChildClass)
{

	CurrentOriginalClass = OriginalClass;

	if (bIsChildClass)
	{

		SetupInterfaces(NewBP, OriginalClass);

		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(NewBP);
	}
	else
	{

		SetupVariables(NewBP, OriginalClass);
		SetupInterfaces(NewBP, OriginalClass);
		SetupComponents(NewBP, OriginalClass);

		if (!bIsLiveUncook)
		{

			SetupCDODefaults(NewBP, OriginalClass);

			FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(NewBP);
		}
	}

	UEdGraph* EventGraph = nullptr;
	if (NewBP->UbergraphPages.Num() > 0)
	{
		EventGraph = NewBP->UbergraphPages[0];
	}
	else
	{
		EventGraph = FBlueprintEditorUtils::CreateNewGraph(NewBP,
			UEdGraphSchema_K2::GN_EventGraph,
			UEdGraph::StaticClass(),
			UEdGraphSchema_K2::StaticClass());
		NewBP->UbergraphPages.Add(EventGraph);
	}

	const FCASDecompiledFunction* UbergraphFunc = nullptr;
	TArray<FCASDecompiledFunction> EventStubs;
	TArray<FCASDecompiledFunction> RegularFunctions;

	for (const FCASDecompiledFunction& Func : Functions)
	{
		if (Func.bIsUbergraph)
		{
			UbergraphFunc = &Func;
		}
		else if (Func.bIsEventStub)
		{
			EventStubs.Add(Func);
		}
		else
		{
			RegularFunctions.Add(Func);
		}
	}

	if (UbergraphFunc)
	{
		BuildEventGraph(NewBP, EventGraph, *UbergraphFunc, EventStubs, OriginalClass);
	}

	EventStubParamNames.Reset();
	for (const FCASDecompiledFunction& Stub : EventStubs)
	{
		if (Stub.OriginalFunction)
		{
			for (TFieldIterator<FProperty> It(Stub.OriginalFunction); It; ++It)
			{
				if (It->HasAnyPropertyFlags(CPF_Parm) && !It->HasAnyPropertyFlags(CPF_ReturnParm))
				{
					EventStubParamNames.Add(It->GetFName());
				}
			}
		}
	}

	for (const FCASDecompiledFunction& Func : RegularFunctions)
	{
		if (Func.Statements.Num() > 0)
		{
			BuildFunctionGraph(NewBP, Func, OriginalClass);
		}
	}

	{
		FKismetEditorUtilities::GenerateBlueprintSkeleton(NewBP,  true);
		FBlueprintEditorUtils::RefreshAllNodes(NewBP);
	}

	{
		TArray<UEdGraph*> AllGraphs;
		AllGraphs.Append(NewBP->UbergraphPages);
		AllGraphs.Append(NewBP->FunctionGraphs);

		for (UEdGraph* Graph : AllGraphs)
		{
			if (!Graph) continue;
			for (UEdGraphNode* Node : Graph->Nodes)
			{
				if (!Node) continue;

				bool bNeedsNotification = false;
				for (UEdGraphPin* Pin : Node->Pins)
				{
					if (!Pin || Pin->LinkedTo.Num() == 0) continue;
					if (Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Wildcard)
					{

						for (UEdGraphPin* LinkedPin : Pin->LinkedTo)
						{
							if (LinkedPin && LinkedPin->PinType.PinCategory != UEdGraphSchema_K2::PC_Wildcard)
							{
								Pin->PinType = LinkedPin->PinType;
								bNeedsNotification = true;
								break;
							}
						}
					}
				}

				if (bNeedsNotification)
				{
					for (UEdGraphPin* Pin : Node->Pins)
					{
						if (Pin && Pin->LinkedTo.Num() > 0)
						{
							Node->PinConnectionListChanged(Pin);
						}
					}
				}
			}
		}
	}

	{
		TArray<UEdGraph*> SelectGraphs;
		SelectGraphs.Append(NewBP->UbergraphPages);
		SelectGraphs.Append(NewBP->FunctionGraphs);

		for (UEdGraph* Graph : SelectGraphs)
		{
			if (!Graph) continue;
			for (UEdGraphNode* Node : Graph->Nodes)
			{
				UK2Node_Select* SelectNode = Cast<UK2Node_Select>(Node);
				if (!SelectNode) continue;

				UEdGraphPin* ReturnPin = SelectNode->GetReturnValuePin();
				if (!ReturnPin) continue;

				FEdGraphPinType ResolvedType = ReturnPin->PinType;
				if (ResolvedType.PinCategory == UEdGraphSchema_K2::PC_Wildcard)
				{
					for (UEdGraphPin* LinkedPin : ReturnPin->LinkedTo)
					{
						if (LinkedPin && LinkedPin->PinType.PinCategory != UEdGraphSchema_K2::PC_Wildcard)
						{
							ResolvedType = LinkedPin->PinType;
							ReturnPin->PinType = ResolvedType;
							break;
						}
					}
				}

				if (ResolvedType.PinCategory == UEdGraphSchema_K2::PC_Wildcard)
				{
					TArray<UEdGraphPin*> OptionPins;
					SelectNode->GetOptionPins(OptionPins);
					for (UEdGraphPin* OptPin : OptionPins)
					{
						if (OptPin)
						{
							for (UEdGraphPin* LinkedPin : OptPin->LinkedTo)
							{
								if (LinkedPin && LinkedPin->PinType.PinCategory != UEdGraphSchema_K2::PC_Wildcard)
								{
									ResolvedType = LinkedPin->PinType;
									ReturnPin->PinType = ResolvedType;
									break;
								}
							}
						}
						if (ResolvedType.PinCategory != UEdGraphSchema_K2::PC_Wildcard)
							break;
					}
				}

				if (ResolvedType.PinCategory == UEdGraphSchema_K2::PC_Wildcard)
					continue; 

				TArray<UEdGraphPin*> OptionPins;
				SelectNode->GetOptionPins(OptionPins);
				for (UEdGraphPin* OptionPin : OptionPins)
				{
					if (OptionPin && OptionPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Wildcard)
					{
						OptionPin->PinType = ResolvedType;
					}
				}
			}
		}
	}

	{
		TArray<UEdGraph*> SanitizeGraphs;
		SanitizeGraphs.Append(NewBP->UbergraphPages);
		SanitizeGraphs.Append(NewBP->FunctionGraphs);

		for (UEdGraph* Graph : SanitizeGraphs)
		{
			SanitizeGraphConnections(Graph);
		}
	}

	{
		int32 RestoredCount = 0;
		for (const FDeferredPinDefault& Deferred : DeferredPinDefaults)
		{
			UEdGraphNode* Node = Deferred.Node.Get();
			if (!Node) continue;
			UEdGraphPin* Pin = Node->FindPin(Deferred.PinName);
			if (!Pin || Pin->LinkedTo.Num() > 0) continue;
			if (UE427IsEmpty(Pin->DefaultValue) && !UE427IsEmpty(Deferred.DefaultValue))
			{
				Pin->DefaultValue = Deferred.DefaultValue;
				RestoredCount++;
			}
			if (!Pin->DefaultObject && Deferred.DefaultObject.IsValid())
			{
				Pin->DefaultObject = Deferred.DefaultObject.Get();
				RestoredCount++;
			}
		}
		if (RestoredCount > 0)
		{
			UE_LOG(LogGraphBuilder, Log, TEXT("Restored %d deferred pin defaults after RefreshAllNodes"), RestoredCount);
		}
		DeferredPinDefaults.Reset();
	}

	if (NewBP->Timelines.Num() > 0)
	{
		TSet<FName> TimelinePropertyNames;
		for (UTimelineTemplate* Timeline : NewBP->Timelines)
		{
			if (!Timeline) continue;

			FName DirPropName = Timeline->GetDirectionPropertyName();
			if (DirPropName != NAME_None)
			{
				TimelinePropertyNames.Add(DirPropName);
			}

			for (const FTTFloatTrack& Track : Timeline->FloatTracks)
			{
				FName PropName = Track.GetPropertyName();
				if (PropName != NAME_None)
				{
					TimelinePropertyNames.Add(PropName);
				}
			}

			for (const FTTVectorTrack& Track : Timeline->VectorTracks)
			{
				FName PropName = Track.GetPropertyName();
				if (PropName != NAME_None)
				{
					TimelinePropertyNames.Add(PropName);
				}
			}

			for (const FTTLinearColorTrack& Track : Timeline->LinearColorTracks)
			{
				FName PropName = Track.GetPropertyName();
				if (PropName != NAME_None)
				{
					TimelinePropertyNames.Add(PropName);
				}
			}
		}

		int32 RemovedCount = 0;
		for (int32 i = NewBP->NewVariables.Num() - 1; i >= 0; --i)
		{
			if (TimelinePropertyNames.Contains(NewBP->NewVariables[i].VarName))
			{
				UE_LOG(LogGraphBuilder, Log, TEXT("Removing timeline-internal property '%s' from NewVariables (will be created from template)"),
					*NewBP->NewVariables[i].VarName.ToString());
				NewBP->NewVariables.RemoveAt(i);
				RemovedCount++;
			}
		}

		if (RemovedCount > 0)
		{
			UE_LOG(LogGraphBuilder, Log, TEXT("Removed %d timeline-internal properties from NewVariables to prevent compiler duplicates"), RemovedCount);
		}
	}

	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(NewBP);

	return true;
}

static TArray<UEdGraph*> GatherAllGraphs(UBlueprint* BP)
{
	TArray<UEdGraph*> AllGraphs;
	AllGraphs.Append(BP->UbergraphPages);
	AllGraphs.Append(BP->FunctionGraphs);
	return AllGraphs;
}

TArray<FCASGraphBuilder::FPinSnapshot> FCASGraphBuilder::SnapshotAllConnections(UBlueprint* BP)
{
	TArray<FPinSnapshot> Snapshot;
	if (!BP) return Snapshot;

	TArray<UEdGraph*> AllGraphs = GatherAllGraphs(BP);

	for (UEdGraph* Graph : AllGraphs)
	{
		if (!Graph) continue;
		for (UEdGraphNode* Node : Graph->Nodes)
		{
			if (!Node) continue;
			for (UEdGraphPin* Pin : Node->Pins)
			{
				if (!Pin) continue;

				bool bHasConnections = Pin->LinkedTo.Num() > 0;
				bool bHasDefaults = !UE427IsEmpty(Pin->DefaultValue) ||
					Pin->DefaultObject != nullptr ||
					!UE427IsEmpty(Pin->DefaultTextValue);

				if (bHasConnections && Pin->Direction != EGPD_Output)
					continue; 
				if (!bHasConnections && !bHasDefaults)
					continue; 

				FPinSnapshot Record;
				Record.OwningNodeGuid = Node->NodeGuid;
				Record.PinName = Pin->GetFName();
				Record.Direction = Pin->Direction;
				Record.DefaultValue = Pin->DefaultValue;
				Record.DefaultObject = Pin->DefaultObject;
				Record.DefaultTextValue = Pin->DefaultTextValue;

				for (UEdGraphPin* LinkedPin : Pin->LinkedTo)
				{
					if (LinkedPin && LinkedPin->GetOwningNode())
					{
						Record.LinkedPinIds.Emplace(
							LinkedPin->GetOwningNode()->NodeGuid,
							LinkedPin->GetFName());
					}
				}

				if (Record.LinkedPinIds.Num() > 0 || bHasDefaults)
				{
					Snapshot.Add(MoveTemp(Record));
				}
			}
		}
	}

	UE_LOG(LogGraphBuilder, Log, TEXT("Snapshot captured %d pin records"), Snapshot.Num());
	return Snapshot;
}

void FCASGraphBuilder::RestoreConnectionsFromSnapshot(UBlueprint* BP, const TArray<FPinSnapshot>& Snapshot)
{
	if (!BP || Snapshot.Num() == 0) return;

	TMap<FGuid, UEdGraphNode*> NodeLookup;
	TArray<UEdGraph*> AllGraphs = GatherAllGraphs(BP);

	for (UEdGraph* Graph : AllGraphs)
	{
		if (!Graph) continue;
		for (UEdGraphNode* Node : Graph->Nodes)
		{
			if (Node)
			{
				NodeLookup.Add(Node->NodeGuid, Node);
			}
		}
	}

	int32 RestoredConnections = 0;
	int32 RestoredDefaults = 0;
	int32 FailedConnections = 0;

	for (const FPinSnapshot& Record : Snapshot)
	{
		UEdGraphNode** OwnerNodePtr = NodeLookup.Find(Record.OwningNodeGuid);
		if (!OwnerNodePtr || !*OwnerNodePtr) continue;

		UEdGraphPin* SourcePin = (*OwnerNodePtr)->FindPin(Record.PinName, Record.Direction);
		if (!SourcePin)
		{
			FailedConnections += Record.LinkedPinIds.Num();
			continue;
		}

		for (const auto& LinkedPinInfo : Record.LinkedPinIds)
		{
			UEdGraphNode** TargetNodePtr = NodeLookup.Find(LinkedPinInfo.Key);
			if (!TargetNodePtr || !*TargetNodePtr)
			{
				FailedConnections++;
				continue;
			}

			EEdGraphPinDirection TargetDir =
				(Record.Direction == EGPD_Input) ? EGPD_Output : EGPD_Input;
			UEdGraphPin* TargetPin = (*TargetNodePtr)->FindPin(LinkedPinInfo.Value, TargetDir);
			if (!TargetPin)
			{

				TargetPin = (*TargetNodePtr)->FindPin(LinkedPinInfo.Value);
			}

			if (!TargetPin)
			{
				FailedConnections++;
				continue;
			}

			if (!SourcePin->LinkedTo.Contains(TargetPin))
			{
				SourcePin->MakeLinkTo(TargetPin);
				RestoredConnections++;
			}
		}

		if (SourcePin->LinkedTo.Num() == 0)
		{
			if (UE427IsEmpty(SourcePin->DefaultValue) && !UE427IsEmpty(Record.DefaultValue))
			{
				SourcePin->DefaultValue = Record.DefaultValue;
				RestoredDefaults++;
			}
			if (!SourcePin->DefaultObject && Record.DefaultObject.IsValid())
			{
				SourcePin->DefaultObject = Record.DefaultObject.Get();
				RestoredDefaults++;
			}
			if (UE427IsEmpty(SourcePin->DefaultTextValue) && !UE427IsEmpty(Record.DefaultTextValue))
			{
				SourcePin->DefaultTextValue = Record.DefaultTextValue;
				RestoredDefaults++;
			}
		}
	}

	UE_LOG(LogGraphBuilder, Log, TEXT("Two-pass restore: %d connections restored, %d defaults restored, %d connections failed"),
		RestoredConnections, RestoredDefaults, FailedConnections);
}

void FCASGraphBuilder::RunPostRefreshFixups(UBlueprint* BP)
{
	if (!BP) return;

	TArray<UEdGraph*> AllGraphs = GatherAllGraphs(BP);

	for (UEdGraph* Graph : AllGraphs)
	{
		if (!Graph) continue;
		for (UEdGraphNode* Node : Graph->Nodes)
		{
			if (!Node) continue;

			bool bNeedsNotification = false;
			for (UEdGraphPin* Pin : Node->Pins)
			{
				if (!Pin || Pin->LinkedTo.Num() == 0) continue;
				if (Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Wildcard)
				{
					for (UEdGraphPin* LinkedPin : Pin->LinkedTo)
					{
						if (LinkedPin && LinkedPin->PinType.PinCategory != UEdGraphSchema_K2::PC_Wildcard)
						{
							Pin->PinType = LinkedPin->PinType;
							bNeedsNotification = true;
							break;
						}
					}
				}
			}

			if (bNeedsNotification)
			{
				for (UEdGraphPin* Pin : Node->Pins)
				{
					if (Pin && Pin->LinkedTo.Num() > 0)
					{
						Node->PinConnectionListChanged(Pin);
					}
				}
			}
		}
	}

	for (UEdGraph* Graph : AllGraphs)
	{
		if (!Graph) continue;
		for (UEdGraphNode* Node : Graph->Nodes)
		{
			UK2Node_Select* SelectNode = Cast<UK2Node_Select>(Node);
			if (!SelectNode) continue;

			UEdGraphPin* ReturnPin = SelectNode->GetReturnValuePin();
			if (!ReturnPin) continue;

			FEdGraphPinType ResolvedType = ReturnPin->PinType;
			if (ResolvedType.PinCategory == UEdGraphSchema_K2::PC_Wildcard)
			{
				for (UEdGraphPin* LinkedPin : ReturnPin->LinkedTo)
				{
					if (LinkedPin && LinkedPin->PinType.PinCategory != UEdGraphSchema_K2::PC_Wildcard)
					{
						ResolvedType = LinkedPin->PinType;
						ReturnPin->PinType = ResolvedType;
						break;
					}
				}
			}

			if (ResolvedType.PinCategory == UEdGraphSchema_K2::PC_Wildcard)
			{
				TArray<UEdGraphPin*> OptionPins;
				SelectNode->GetOptionPins(OptionPins);
				for (UEdGraphPin* OptPin : OptionPins)
				{
					if (OptPin)
					{
						for (UEdGraphPin* LinkedPin : OptPin->LinkedTo)
						{
							if (LinkedPin && LinkedPin->PinType.PinCategory != UEdGraphSchema_K2::PC_Wildcard)
							{
								ResolvedType = LinkedPin->PinType;
								ReturnPin->PinType = ResolvedType;
								break;
							}
						}
					}
					if (ResolvedType.PinCategory != UEdGraphSchema_K2::PC_Wildcard)
						break;
				}
			}

			if (ResolvedType.PinCategory == UEdGraphSchema_K2::PC_Wildcard)
				continue;

			TArray<UEdGraphPin*> OptionPins;
			SelectNode->GetOptionPins(OptionPins);
			for (UEdGraphPin* OptionPin : OptionPins)
			{
				if (OptionPin && OptionPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Wildcard)
				{
					OptionPin->PinType = ResolvedType;
				}
			}
		}
	}

	for (UEdGraph* Graph : AllGraphs)
	{
		SanitizeGraphConnections(Graph);
	}
}

TArray<FCASGraphBuilder::FEventRefSnapshot> FCASGraphBuilder::SnapshotEventReferences(UBlueprint* BP)
{
	TArray<FEventRefSnapshot> Snapshot;
	if (!BP) return Snapshot;

	TArray<UEdGraph*> AllGraphs = GatherAllGraphs(BP);

	for (UEdGraph* Graph : AllGraphs)
	{
		if (!Graph) continue;
		for (UEdGraphNode* Node : Graph->Nodes)
		{
			UK2Node_Event* EventNode = Cast<UK2Node_Event>(Node);
			if (!EventNode) continue;

			FEventRefSnapshot Record;
			Record.NodeGuid = EventNode->NodeGuid;
			Record.EventName = EventNode->EventReference.GetMemberName();

			UClass* MemberClass = EventNode->EventReference.GetMemberParentClass();
			if (MemberClass)
			{
				Record.DeclaringClass = MemberClass;
				Snapshot.Add(Record);
			}
		}
	}

	UE_LOG(LogGraphBuilder, Log, TEXT("Snapshot captured %d event references"), Snapshot.Num());
	return Snapshot;
}

void FCASGraphBuilder::RestoreEventReferences(UBlueprint* BP, const TArray<FEventRefSnapshot>& Snapshot)
{
	if (!BP || Snapshot.Num() == 0) return;

	TMap<FGuid, const FEventRefSnapshot*> SnapshotLookup;
	for (const FEventRefSnapshot& Record : Snapshot)
	{
		SnapshotLookup.Add(Record.NodeGuid, &Record);
	}

	TArray<UEdGraph*> AllGraphs = GatherAllGraphs(BP);
	int32 RestoredCount = 0;

	for (UEdGraph* Graph : AllGraphs)
	{
		if (!Graph) continue;
		for (UEdGraphNode* Node : Graph->Nodes)
		{
			UK2Node_Event* EventNode = Cast<UK2Node_Event>(Node);
			if (!EventNode) continue;

			const FEventRefSnapshot** RecordPtr = SnapshotLookup.Find(EventNode->NodeGuid);
			if (!RecordPtr || !*RecordPtr) continue;

			const FEventRefSnapshot& Record = **RecordPtr;
			UClass* OrigDeclaringClass = Record.DeclaringClass.Get();
			if (!OrigDeclaringClass) continue;

			UClass* CurrentClass = EventNode->EventReference.GetMemberParentClass();
			if (CurrentClass != OrigDeclaringClass)
			{
				EventNode->EventReference.SetExternalMember(Record.EventName, OrigDeclaringClass);
				RestoredCount++;
			}
		}
	}

	UE_LOG(LogGraphBuilder, Log, TEXT("Restored %d event references after RefreshAllNodes"), RestoredCount);
}

void FCASGraphBuilder::SetupVariables(UBlueprint* BP, UClass* OriginalClass)
{

	for (TFieldIterator<FProperty> It(OriginalClass, EFieldIteratorFlags::ExcludeSuper); It; ++It)
	{
		FProperty* Prop = *It;

		if (Prop->GetOwnerClass() != OriginalClass)
			continue;

		if (Prop->HasAnyPropertyFlags(CPF_Parm | CPF_ReturnParm))
			continue;

		if (FObjectProperty* ObjProp = CastField<FObjectProperty>(Prop))
		{
			if (ObjProp->PropertyClass && ObjProp->PropertyClass->IsChildOf(UTimelineComponent::StaticClass()))
			{
				UE_LOG(LogGraphBuilder, Verbose, TEXT("Skipping timeline component variable: %s (owned by UK2Node_Timeline)"),
					*Prop->GetName());
				continue;
			}
		}

		bool bAlreadyExists = false;
		for (const FBPVariableDescription& ExistingVar : BP->NewVariables)
		{
			if (ExistingVar.VarName == Prop->GetFName())
			{
				bAlreadyExists = true;
				break;
			}
		}
		if (bAlreadyExists) continue;

		FBPVariableDescription NewVar;
		NewVar.VarName = Prop->GetFName();
		NewVar.VarGuid = FGuid::NewGuid();
		NewVar.FriendlyName = Prop->GetName();
		NewVar.PropertyFlags = Prop->PropertyFlags;
		NewVar.RepNotifyFunc = NAME_None;

		const UEdGraphSchema_K2* Schema = GetDefault<UEdGraphSchema_K2>();
		Schema->ConvertPropertyToPinType(Prop, NewVar.VarType);

		if (Prop->HasMetaData(TEXT("Category")))
		{
			NewVar.Category = FText::FromString(Prop->GetMetaData(TEXT("Category")));
		}

		BP->NewVariables.Add(NewVar);

		UE_LOG(LogGraphBuilder, Verbose, TEXT("Added variable: %s (%s)"),
			*NewVar.VarName.ToString(), *NewVar.VarType.PinCategory.ToString());
	}

}

void FCASGraphBuilder::SetupInterfaces(UBlueprint* BP, UClass* OriginalClass)
{

	for (const FImplementedInterface& Interface : OriginalClass->Interfaces)
	{
		if (Interface.Class)
		{
			FBPInterfaceDescription NewInterface;
			NewInterface.Interface = Interface.Class;
			BP->ImplementedInterfaces.Add(NewInterface);

			UE_LOG(LogGraphBuilder, Verbose, TEXT("Added interface: %s"),
				*Interface.Class->GetName());
		}
	}
}

void FCASGraphBuilder::SetupComponents(UBlueprint* BP, UClass* OriginalClass)
{
	UBlueprintGeneratedClass* BPGC = Cast<UBlueprintGeneratedClass>(OriginalClass);
	if (!BPGC) return;

	USimpleConstructionScript* OriginalSCS = BPGC->SimpleConstructionScript;
	if (OriginalSCS)
	{
		UE_LOG(LogGraphBuilder, Log, TEXT("Cloning SimpleConstructionScript (%d root nodes)"),
			OriginalSCS->GetRootNodes().Num());

		UObject* SCSOwner = BP->GeneratedClass ? (UObject*)BP->GeneratedClass : (UObject*)BP;
		USimpleConstructionScript* NewSCS = DuplicateObject<USimpleConstructionScript>(OriginalSCS, SCSOwner);
		if (NewSCS)
		{
			BP->SimpleConstructionScript = NewSCS;

			if (UBlueprintGeneratedClass* NewBPGC = Cast<UBlueprintGeneratedClass>(BP->GeneratedClass))
			{
				NewBPGC->SimpleConstructionScript = NewSCS;
			}

			const TArray<USCS_Node*>& AllNodes = NewSCS->GetAllNodes();
			for (USCS_Node* Node : AllNodes)
			{
				if (Node && Node->ComponentTemplate)
				{

					Node->ComponentTemplate->Rename(nullptr, SCSOwner);

					UE_LOG(LogGraphBuilder, Log, TEXT("  Component: %s (%s)"),
						*Node->GetVariableName().ToString(),
						*Node->ComponentTemplate->GetClass()->GetName());
				}
			}

			UE_LOG(LogGraphBuilder, Log, TEXT("SCS cloned with %d total nodes"), AllNodes.Num());
		}
		else
		{
			Warn(TEXT("Failed to duplicate SimpleConstructionScript"));
		}
	}
	else
	{
		UE_LOG(LogGraphBuilder, Log, TEXT("No SimpleConstructionScript found on original class"));

		UObject* OriginalCDO = OriginalClass->GetDefaultObject();
		if (!OriginalCDO) return;

		UObject* SCSOwner = BP->GeneratedClass ? (UObject*)BP->GeneratedClass : (UObject*)BP;
		USimpleConstructionScript* NewSCS = NewObject<USimpleConstructionScript>(SCSOwner);
		BP->SimpleConstructionScript = NewSCS;
		if (UBlueprintGeneratedClass* NewBPGC = Cast<UBlueprintGeneratedClass>(BP->GeneratedClass))
		{
			NewBPGC->SimpleConstructionScript = NewSCS;
		}

		TArray<UObject*> Subobjects;
		OriginalCDO->GetDefaultSubobjects(Subobjects);

		int32 ComponentCount = 0;
		for (UObject* Subobj : Subobjects)
		{
			UActorComponent* CompTemplate = Cast<UActorComponent>(Subobj);
			if (!CompTemplate) continue;

			UObject* Archetype = CompTemplate->GetArchetype();
			if (Archetype && Archetype != CompTemplate &&
				Archetype->GetOuter() != OriginalCDO)
			{
				continue;
			}

			UActorComponent* NewTemplate = DuplicateObject<UActorComponent>(CompTemplate, BP);
			if (!NewTemplate) continue;

			USCS_Node* NewNode = NewSCS->CreateNode(NewTemplate->GetClass(), CompTemplate->GetFName());
			if (NewNode)
			{

				if (NewNode->ComponentTemplate)
				{
					NewNode->ComponentTemplate->MarkPendingKill();
				}
				NewNode->ComponentTemplate = NewTemplate;

				NewSCS->AddNode(NewNode);
				ComponentCount++;

				UE_LOG(LogGraphBuilder, Log, TEXT("  Reconstructed component: %s (%s)"),
					*CompTemplate->GetFName().ToString(),
					*CompTemplate->GetClass()->GetName());
			}
		}

		if (ComponentCount > 0)
		{
			UE_LOG(LogGraphBuilder, Log, TEXT("Reconstructed %d components from CDO subobjects"), ComponentCount);
		}
	}

	if (BPGC->ComponentTemplates.Num() > 0)
	{
		UE_LOG(LogGraphBuilder, Log, TEXT("Copying %d component templates"), BPGC->ComponentTemplates.Num());

		for (UActorComponent* Template : BPGC->ComponentTemplates)
		{
			if (!Template) continue;

			UActorComponent* NewTemplate = DuplicateObject<UActorComponent>(Template, BP);
			if (NewTemplate)
			{
				BP->ComponentTemplates.Add(NewTemplate);
			}
		}
	}

	UE_LOG(LogGraphBuilder, Log, TEXT("BPGC->Timelines count: %d"), BPGC->Timelines.Num());
	if (BPGC->Timelines.Num() > 0)
	{
		UE_LOG(LogGraphBuilder, Log, TEXT("Copying %d timeline templates"), BPGC->Timelines.Num());

		for (UTimelineTemplate* Timeline : BPGC->Timelines)
		{
			if (!Timeline) continue;

			UE_LOG(LogGraphBuilder, Log, TEXT("  Template '%s': VariableName='%s', FloatTracks=%d, VectorTracks=%d, GUID=%s"),
				*Timeline->GetName(),
				*Timeline->GetVariableName().ToString(),
				Timeline->FloatTracks.Num(),
				Timeline->VectorTracks.Num(),
				*Timeline->TimelineGuid.ToString());

			UObject* TimelineOuter = BP->GeneratedClass ? (UObject*)BP->GeneratedClass : (UObject*)BP;

			FGuid OriginalGuid = Timeline->TimelineGuid;

			UTimelineTemplate* NewTimeline = DuplicateObject<UTimelineTemplate>(Timeline, TimelineOuter);
			if (NewTimeline)
			{

				NewTimeline->TimelineGuid = OriginalGuid;
				NewTimeline->PostEditImport(); 

				BP->Timelines.Add(NewTimeline);

				UE_LOG(LogGraphBuilder, Log, TEXT("  -> Copied as '%s' (outer: %s, GUID preserved: %s)"),
					*NewTimeline->GetName(), *TimelineOuter->GetName(), *OriginalGuid.ToString());
				UE_LOG(LogGraphBuilder, Log, TEXT("     VariableName='%s', DirectionProp='%s'"),
					*NewTimeline->GetVariableName().ToString(),
					*NewTimeline->GetDirectionPropertyName().ToString());
				for (int32 TrackIdx = 0; TrackIdx < NewTimeline->FloatTracks.Num(); ++TrackIdx)
				{
					const FTTFloatTrack& Track = NewTimeline->FloatTracks[TrackIdx];
					UE_LOG(LogGraphBuilder, Log, TEXT("     FloatTrack[%d]: TrackName='%s', PropertyName='%s', CurveFloat=%s"),
						TrackIdx, *Track.GetTrackName().ToString(), *Track.GetPropertyName().ToString(),
						Track.CurveFloat ? *Track.CurveFloat->GetPathName() : TEXT("NULL"));
				}
			}
		}
	}
	else
	{

		UE_LOG(LogGraphBuilder, Log, TEXT("No timeline templates in BPGC — attempting reconstruction from cooked data"));

		UObject* CDO = OriginalClass->GetDefaultObject();
		for (TFieldIterator<FObjectProperty> It(OriginalClass, EFieldIteratorFlags::ExcludeSuper); It; ++It)
		{
			FObjectProperty* ObjProp = *It;
			if (!ObjProp->PropertyClass || !ObjProp->PropertyClass->IsChildOf(UTimelineComponent::StaticClass()))
				continue;

			UTimelineComponent* TimelineComp = CDO ? Cast<UTimelineComponent>(ObjProp->GetObjectPropertyValue(ObjProp->ContainerPtrToValuePtr<void>(CDO))) : nullptr;
			FName TimelineVarName = ObjProp->GetFName();
			FString SanitizedName = TimelineVarName.ToString();
			SanitizedName.ReplaceCharInline(TEXT(' '), TEXT('_'));
			SanitizedName.ReplaceCharInline(TEXT('/'), TEXT('_'));

			FString DirectionPrefix = SanitizedName + TEXT("__Direction_");
			FGuid ExtractedGuid;
			FName DirectionPropName = NAME_None;
			for (TFieldIterator<FProperty> PropIt(OriginalClass, EFieldIteratorFlags::ExcludeSuper); PropIt; ++PropIt)
			{
				FString PropName = PropIt->GetName();
				if (PropName.StartsWith(DirectionPrefix))
				{
					FString GuidStr = PropName.Mid(DirectionPrefix.Len());
					FGuid::Parse(GuidStr, ExtractedGuid);
					DirectionPropName = PropIt->GetFName();
					break;
				}
			}

			if (!ExtractedGuid.IsValid())
			{
				UE_LOG(LogGraphBuilder, Warning, TEXT("Could not extract GUID for timeline '%s' — skipping reconstruction"), *TimelineVarName.ToString());
				continue;
			}

			FString TemplateName = UTimelineTemplate::TimelineVariableNameToTemplateName(TimelineVarName);
			UObject* TimelineOuter = BP->GeneratedClass ? (UObject*)BP->GeneratedClass : (UObject*)BP;
			UTimelineTemplate* NewTemplate = NewObject<UTimelineTemplate>(TimelineOuter, *TemplateName);
			NewTemplate->TimelineGuid = ExtractedGuid;
			NewTemplate->TimelineLength = TimelineComp ? TimelineComp->GetTimelineLength() : 1.0f;
			NewTemplate->bAutoPlay = false;
			NewTemplate->bLoop = TimelineComp ? TimelineComp->IsLooping() : false;

			NewTemplate->PostEditImport();

			FString GuidStr = ExtractedGuid.ToString();
			FString TrackSuffix = TEXT("_") + GuidStr;
			FString TrackPrefix = SanitizedName + TEXT("_");
			for (TFieldIterator<FFloatProperty> FloatIt(OriginalClass, EFieldIteratorFlags::ExcludeSuper); FloatIt; ++FloatIt)
			{
				FString FloatPropName = FloatIt->GetName();
				if (FloatPropName.StartsWith(TrackPrefix) && FloatPropName.EndsWith(TrackSuffix) && FloatPropName != DirectionPropName.ToString())
				{

					FString TrackName = FloatPropName.Mid(TrackPrefix.Len(), FloatPropName.Len() - TrackPrefix.Len() - TrackSuffix.Len());

					FTTFloatTrack NewTrack;
					NewTrack.SetTrackName(FName(*TrackName), NewTemplate);

					if (TimelineComp)
					{

						FStructProperty* TimelineProp = CastField<FStructProperty>(
							UTimelineComponent::StaticClass()->FindPropertyByName(TEXT("TheTimeline")));
						if (TimelineProp)
						{
							void* TimelinePtr = TimelineProp->ContainerPtrToValuePtr<void>(TimelineComp);

							FArrayProperty* FloatsProp = CastField<FArrayProperty>(
								TimelineProp->Struct->FindPropertyByName(TEXT("InterpFloats")));
							if (FloatsProp && TimelinePtr)
							{
								FScriptArrayHelper ArrayHelper(FloatsProp, FloatsProp->ContainerPtrToValuePtr<void>(TimelinePtr));
								FStructProperty* ElemProp = CastField<FStructProperty>(FloatsProp->Inner);
								for (int32 Idx = 0; Idx < ArrayHelper.Num(); ++Idx)
								{
									void* ElemPtr = ArrayHelper.GetRawPtr(Idx);

									FNameProperty* TrackNameProp = CastField<FNameProperty>(
										ElemProp->Struct->FindPropertyByName(TEXT("TrackName")));
									if (TrackNameProp)
									{
										FName CompTrackName = TrackNameProp->GetPropertyValue(
											TrackNameProp->ContainerPtrToValuePtr<void>(ElemPtr));
										if (CompTrackName.ToString() == TrackName)
										{

											FObjectProperty* CurveProp = CastField<FObjectProperty>(
												ElemProp->Struct->FindPropertyByName(TEXT("FloatCurve")));
											if (CurveProp)
											{
												UCurveFloat* Curve = Cast<UCurveFloat>(
													CurveProp->GetObjectPropertyValue(
														CurveProp->ContainerPtrToValuePtr<void>(ElemPtr)));
												if (Curve)
												{
													NewTrack.CurveFloat = Curve;
													UE_LOG(LogGraphBuilder, Log, TEXT("    Found curve asset: %s"),
														*Curve->GetPathName());
												}
											}
											break;
										}
									}
								}
							}
						}
					}

					NewTemplate->FloatTracks.Add(NewTrack);
					UE_LOG(LogGraphBuilder, Log, TEXT("  Reconstructed float track: %s (property: %s)"),
						*TrackName, *FloatPropName);
				}
			}

			NewTemplate->PostEditImport();

			BP->Timelines.Add(NewTemplate);
			UE_LOG(LogGraphBuilder, Log, TEXT("Reconstructed timeline template '%s' (GUID: %s, %d float tracks)"),
				*TemplateName, *GuidStr, NewTemplate->FloatTracks.Num());
		}
	}
}

void FCASGraphBuilder::SetupCDODefaults(UBlueprint* BP, UClass* OriginalClass)
{
	UObject* OriginalCDO = OriginalClass->GetDefaultObject();
	if (!OriginalCDO) return;

	for (FBPVariableDescription& Var : BP->NewVariables)
	{
		FProperty* OrigProp = OriginalClass->FindPropertyByName(Var.VarName);
		if (!OrigProp) continue;

		FString DefaultValueStr;
		const void* PropAddr = OrigProp->ContainerPtrToValuePtr<void>(OriginalCDO);

		OrigProp->ExportTextItem(DefaultValueStr, PropAddr, nullptr, OriginalCDO, PPF_None);

		if (!UE427IsEmpty(DefaultValueStr))
		{
			FStructProperty* StructProp = CastField<FStructProperty>(OrigProp);
			if (StructProp && StructProp->Struct)
			{
				FString StructName = StructProp->Struct->GetName();

				if (StructName == TEXT("Transform") || StructName == TEXT("Quat"))
				{
					continue;  
				}

				if (StructName == TEXT("Rotator") && DefaultValueStr.Contains(TEXT("Pitch=")))
				{

					float Pitch = 0, Yaw = 0, Roll = 0;
					FParse::Value(*DefaultValueStr, TEXT("Pitch="), Pitch);
					FParse::Value(*DefaultValueStr, TEXT("Yaw="), Yaw);
					FParse::Value(*DefaultValueStr, TEXT("Roll="), Roll);
					DefaultValueStr = FString::Printf(TEXT("%f,%f,%f"), Pitch, Yaw, Roll);
				}

				else if (StructName == TEXT("Vector") && DefaultValueStr.Contains(TEXT("X=")))
				{
					FVector Vec;
					if (Vec.InitFromString(DefaultValueStr))
					{
						DefaultValueStr = FString::Printf(TEXT("%f,%f,%f"), Vec.X, Vec.Y, Vec.Z);
					}
				}
			}

			Var.DefaultValue = DefaultValueStr;
		}
	}

	UE_LOG(LogGraphBuilder, Log, TEXT("Copied CDO default values for %d variables"), BP->NewVariables.Num());
}

void FCASGraphBuilder::BuildEventGraph(
	UBlueprint* BP,
	UEdGraph* EventGraph,
	const FCASDecompiledFunction& UbergraphFunc,
	const TArray<FCASDecompiledFunction>& EventStubs,
	UClass* OriginalClass)
{
	UE_LOG(LogGraphBuilder, Log, TEXT("Building event graph (%d statements, %d events)"),
		UbergraphFunc.Statements.Num(), EventStubs.Num());

	PersistentFrameVarMap.Reset();
	DeferredEventParameterWires.Reset();

	TMap<FString, int32> BoundEventSignatureCounters;

	TArray<TPair<FName, int32>> SortedEntries;
	for (const auto& Entry : UbergraphFunc.EventEntryPoints)
	{
		SortedEntries.Add(TPair<FName, int32>(Entry.Key, Entry.Value));
	}
	SortedEntries.Sort([](const TPair<FName, int32>& A, const TPair<FName, int32>& B)
		{
			return A.Value < B.Value;
		});

	TMap<int32, int32> OffsetToStmtIdx;
	for (int32 i = 0; i < UbergraphFunc.Statements.Num(); i++)
	{
		if (UbergraphFunc.Statements[i])
		{
			OffsetToStmtIdx.Add(UbergraphFunc.Statements[i]->StartOffset, i);
		}
	}

	TMap<FName, TSet<int32>> EventReachableSets;

	for (const auto& Entry : SortedEntries)
	{
		const FName& EvtName = Entry.Key;
		int32 EvtEntryOffset = Entry.Value;

		TSet<int32> Reachable;
		TSet<int32> VisitedOffsets;
		TArray<int32> WorkList;

		int32* StartIdxPtr = OffsetToStmtIdx.Find(EvtEntryOffset);
		if (StartIdxPtr)
		{
			WorkList.Add(EvtEntryOffset);
			VisitedOffsets.Add(EvtEntryOffset);
		}

		while (WorkList.Num() > 0)
		{
			int32 CurrOffset = WorkList.Pop();
			int32* CurrIdxPtr = OffsetToStmtIdx.Find(CurrOffset);
			if (!CurrIdxPtr) continue;
			int32 CurrIdx = *CurrIdxPtr;
			if (CurrIdx < 0 || CurrIdx >= UbergraphFunc.Statements.Num()) continue;

			Reachable.Add(CurrIdx);
			const TSharedPtr<FCASDecompiledExpr>& Stmt = UbergraphFunc.Statements[CurrIdx];
			if (!Stmt) continue;

			auto AddSuccessor = [&](int32 Offset)
			{
				if (!VisitedOffsets.Contains(Offset))
				{
					VisitedOffsets.Add(Offset);
					WorkList.Add(Offset);
				}
			};

			auto AddFallThrough = [&]()
			{
				int32 NextIdx = CurrIdx + 1;
				if (NextIdx < UbergraphFunc.Statements.Num() && UbergraphFunc.Statements[NextIdx])
				{
					AddSuccessor(UbergraphFunc.Statements[NextIdx]->StartOffset);
				}
			};

			switch (Stmt->Token)
			{
			case EX_Return:
			case EX_EndOfScript:
			case EX_PopExecutionFlow:

				break;

			case EX_Jump:
				AddSuccessor(Stmt->JumpTarget);
				break;

			case EX_JumpIfNot:
				AddSuccessor(Stmt->JumpTarget);  
				AddFallThrough();                
				break;

			case EX_PopExecutionFlowIfNot:

				AddFallThrough();

				break;

			case EX_PushExecutionFlow:
				AddSuccessor(Stmt->JumpTarget);  
				AddFallThrough();                
				break;

			case EX_SwitchValue:
				AddSuccessor(Stmt->JumpTarget);  
				break;

			default:

				AddFallThrough();

				for (const TSharedPtr<FCASDecompiledExpr>& Child : Stmt->Children)
				{
					if (Child && Child->Token == EX_StructConst && Child->StructRef &&
						Child->StructRef->GetName() == TEXT("LatentActionInfo"))
					{
						for (const TSharedPtr<FCASDecompiledExpr>& StructChild : Child->Children)
						{
							if (StructChild && StructChild->Token == EX_SkipOffsetConst && StructChild->JumpTarget > 0)
							{
								AddSuccessor(StructChild->JumpTarget);
								break;
							}
						}
						break;
					}
				}
				break;
			}
		}

		EventReachableSets.Add(EvtName, MoveTemp(Reachable));

		UE_LOG(LogGraphBuilder, Log, TEXT("Event '%s' at offset 0x%04X: %d reachable statements"),
			*EvtName.ToString(), EvtEntryOffset, EventReachableSets[EvtName].Num());
	}

	int32 EventY = 0;

	TMap<FString, UK2Node_Timeline*> TimelineNodes;

	TMap<FString, int32> LastInputEventStubIndex;

	for (int32 EventIdx = 0; EventIdx < SortedEntries.Num(); EventIdx++)
	{
		const FName& EventName = SortedEntries[EventIdx].Key;
		int32 EntryOffset = SortedEntries[EventIdx].Value;

		const TSet<int32>* pReachable = EventReachableSets.Find(EventName);
		if (!pReachable || pReachable->Num() == 0)
		{
			UE_LOG(LogGraphBuilder, Log, TEXT("Event '%s' has no reachable statements at offset 0x%04X — skipping"),
				*EventName.ToString(), EntryOffset);
			continue;
		}

		int32 StartStatementIdx = UbergraphFunc.Statements.Num();
		int32 EndStatementIdx = 0;
		for (int32 StmtIdx : *pReachable)
		{
			StartStatementIdx = FMath::Min(StartStatementIdx, StmtIdx);
			EndStatementIdx = FMath::Max(EndStatementIdx, StmtIdx);
		}
		EndStatementIdx += 1; 

		UFunction* StubFunction = nullptr;
		for (const FCASDecompiledFunction& Stub : EventStubs)
		{
			if (Stub.OriginalFunction->GetFName() == EventName)
			{
				StubFunction = Stub.OriginalFunction;
				break;
			}
		}

		UEdGraphNode* EventNode = nullptr;
		UEdGraphPin* EventExecOut = nullptr;

		FString EventNameStr = EventName.ToString();
		bool bHandledAsInputEvent = false;

		if (EventNameStr.StartsWith(TEXT("InpActEvt_")))
		{

			FString Remainder = EventNameStr.Mid(10); 

			FString InputName;
			int32 StubIndex = 0;
			bool bIsKeyEvent = false;

			int32 SepIdx = Remainder.Find(TEXT("_K2Node_InputActionEvent_"));
			if (SepIdx != INDEX_NONE)
			{
				InputName = Remainder.Left(SepIdx);
				StubIndex = FCString::Atoi(*Remainder.Mid(SepIdx + 25));
			}
			else
			{
				SepIdx = Remainder.Find(TEXT("_K2Node_InputKeyEvent_"));
				if (SepIdx != INDEX_NONE)
				{
					InputName = Remainder.Left(SepIdx);
					StubIndex = FCString::Atoi(*Remainder.Mid(SepIdx + 22));
					bIsKeyEvent = true;
				}
			}

			if (!UE427IsEmpty(InputName))
			{

				EInputEvent InputEventType = IE_Pressed;
				FString TrackKey = InputName + (bIsKeyEvent ? TEXT("_Key") : TEXT("_Action"));
				if (int32* PrevIdx = LastInputEventStubIndex.Find(TrackKey))
				{
					if (StubIndex == *PrevIdx + 1)
					{
						InputEventType = IE_Released;
					}
				}
				LastInputEventStubIndex.FindOrAdd(TrackKey) = StubIndex;

				if (bIsKeyEvent)
				{

					UK2Node_InputKey* InputKeyNode = NewObject<UK2Node_InputKey>(EventGraph);
					InputKeyNode->CreateNewGuid();
					InputKeyNode->InputKey = FKey(*InputName);
					InputKeyNode->NodePosX = 0;
					InputKeyNode->NodePosY = EventY;
					InputKeyNode->AllocateDefaultPins();
					EventGraph->AddNode(InputKeyNode, false, false);
					EventNode = InputKeyNode;
				}
				else
				{

					UK2Node_InputAction* InputActionNode = NewObject<UK2Node_InputAction>(EventGraph);
					InputActionNode->CreateNewGuid();
					InputActionNode->InputActionName = FName(*InputName);
					InputActionNode->NodePosX = 0;
					InputActionNode->NodePosY = EventY;
					InputActionNode->AllocateDefaultPins();
					EventGraph->AddNode(InputActionNode, false, false);
					EventNode = InputActionNode;
				}

				FName ExecPinName = (InputEventType == IE_Released) ? FName(TEXT("Released")) : FName(TEXT("Pressed"));
				for (UEdGraphPin* Pin : EventNode->Pins)
				{
					if (Pin->Direction == EGPD_Output && Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Exec)
					{
						if (Pin->PinName == ExecPinName)
						{
							EventExecOut = Pin;
							break;
						}
					}
				}
				if (!EventExecOut)
				{
					EventExecOut = FindExecPin(EventNode, EGPD_Output);
				}

				bHandledAsInputEvent = true;
				UE_LOG(LogGraphBuilder, Log, TEXT("Input %s event: %s (%s)"),
					bIsKeyEvent ? TEXT("Key") : TEXT("Action"),
					*InputName,
					(InputEventType == IE_Released) ? TEXT("Released") : TEXT("Pressed"));
			}
		}
		else if (EventNameStr.StartsWith(TEXT("InpAxisEvt_")))
		{

			FString Remainder = EventNameStr.Mid(11); 

			FString AxisName;
			int32 SepIdx = Remainder.Find(TEXT("_K2Node_InputAxisEvent_"));
			if (SepIdx != INDEX_NONE)
			{
				AxisName = Remainder.Left(SepIdx);
			}

			if (!UE427IsEmpty(AxisName))
			{
				UK2Node_InputAxisEvent* InputAxisNode = NewObject<UK2Node_InputAxisEvent>(EventGraph);
				InputAxisNode->CreateNewGuid();
				InputAxisNode->InputAxisName = FName(*AxisName);
				InputAxisNode->NodePosX = 0;
				InputAxisNode->NodePosY = EventY;
				InputAxisNode->AllocateDefaultPins();
				EventGraph->AddNode(InputAxisNode, false, false);
				EventNode = InputAxisNode;
				EventExecOut = FindExecPin(InputAxisNode, EGPD_Output);

				bHandledAsInputEvent = true;
				UE_LOG(LogGraphBuilder, Log, TEXT("Input Axis event: %s"), *AxisName);
			}
		}
		else if (EventNameStr.StartsWith(TEXT("InpAxisKeyEvt_")))
		{

			FString Remainder = EventNameStr.Mid(14); 

			FString KeyName;
			int32 SepIdx = Remainder.Find(TEXT("_K2Node_InputAxisKeyEvent_"));
			if (SepIdx != INDEX_NONE)
			{
				KeyName = Remainder.Left(SepIdx);
			}

			if (!UE427IsEmpty(KeyName))
			{
				UK2Node_InputAxisKeyEvent* InputAxisKeyNode = NewObject<UK2Node_InputAxisKeyEvent>(EventGraph);
				InputAxisKeyNode->CreateNewGuid();
				InputAxisKeyNode->AxisKey = FKey(*KeyName);
				InputAxisKeyNode->NodePosX = 0;
				InputAxisKeyNode->NodePosY = EventY;
				InputAxisKeyNode->AllocateDefaultPins();
				EventGraph->AddNode(InputAxisKeyNode, false, false);
				EventNode = InputAxisKeyNode;
				EventExecOut = FindExecPin(InputAxisKeyNode, EGPD_Output);

				bHandledAsInputEvent = true;
				UE_LOG(LogGraphBuilder, Log, TEXT("Input Axis Key event: %s"), *KeyName);
			}
		}
		else if (EventNameStr.Contains(TEXT("__UpdateFunc")) || EventNameStr.Contains(TEXT("__FinishedFunc")))
		{

			bool bIsUpdate = EventNameStr.Contains(TEXT("__UpdateFunc"));
			FString TimelineName;
			if (bIsUpdate)
			{
				int32 SepIdx = EventNameStr.Find(TEXT("__UpdateFunc"));
				if (SepIdx != INDEX_NONE) TimelineName = EventNameStr.Left(SepIdx);
			}
			else
			{
				int32 SepIdx = EventNameStr.Find(TEXT("__FinishedFunc"));
				if (SepIdx != INDEX_NONE) TimelineName = EventNameStr.Left(SepIdx);
			}

			if (!UE427IsEmpty(TimelineName))
			{

				UK2Node_Timeline** ExistingPtr = TimelineNodes.Find(TimelineName);
				UK2Node_Timeline* TimelineNode = ExistingPtr ? *ExistingPtr : nullptr;

				if (!TimelineNode)
				{
					TimelineNode = NewObject<UK2Node_Timeline>(EventGraph);
					TimelineNode->CreateNewGuid();
					TimelineNode->TimelineName = FName(*TimelineName);
					TimelineNode->NodePosX = 0;
					TimelineNode->NodePosY = EventY;
					TimelineNode->AllocateDefaultPins();
					EventGraph->AddNode(TimelineNode, false, false);
					TimelineNodes.Add(TimelineName, TimelineNode);

					UE_LOG(LogGraphBuilder, Log, TEXT("Created Timeline node: %s"), *TimelineName);
				}

				EventNode = TimelineNode;

				FName PinName = bIsUpdate ? FName(TEXT("Update")) : FName(TEXT("Finished"));
				for (UEdGraphPin* Pin : TimelineNode->Pins)
				{
					if (Pin->Direction == EGPD_Output &&
						Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Exec &&
						Pin->PinName == PinName)
					{
						EventExecOut = Pin;
						break;
					}
				}
				if (!EventExecOut)
				{
					EventExecOut = FindExecPin(TimelineNode, EGPD_Output);
				}

				bHandledAsInputEvent = true;
				UE_LOG(LogGraphBuilder, Log, TEXT("Timeline %s event: %s"),
					bIsUpdate ? TEXT("Update") : TEXT("Finished"), *TimelineName);
			}
		}

		UFunction* ParentFunc = nullptr;
		bool bIsOverrideEvent = false;

		if (!bHandledAsInputEvent)
		{

		bool bHandledAsComponentBoundEvent = false;
		if (EventNameStr.StartsWith(TEXT("BndEvt__")))
		{

			FString AfterPrefix = EventNameStr.Mid(8); 
			int32 BoundEventMarker = AfterPrefix.Find(TEXT("_K2Node_ComponentBoundEvent_"));

			if (BoundEventMarker != INDEX_NONE)
			{
				FString ComponentName = AfterPrefix.Left(BoundEventMarker);

				FString OriginalComponentName = ComponentName; 
				bool bStrippedPrefix = false;
				for (UClass* TestClass = OriginalClass; TestClass && !bStrippedPrefix; TestClass = TestClass->GetSuperClass())
				{
					FString TestClassName = TestClass->GetName();
					TestClassName.RemoveFromEnd(TEXT("_C"));
					if (ComponentName.StartsWith(TestClassName + TEXT("_")))
					{
						ComponentName = ComponentName.Mid(TestClassName.Len() + 1);
						bStrippedPrefix = true;
					}
				}
				FString AfterMarker = AfterPrefix.Mid(BoundEventMarker + 28); 

				int32 FirstUnderscore;
				if (AfterMarker.FindChar(TEXT('_'), FirstUnderscore))
				{
					FString DelegateSigFuncName = AfterMarker.Mid(FirstUnderscore + 1);
					const TPair<FName, FName>* Binding = ComponentDelegateBindings.Find(FName(*EventNameStr));
					if (Binding) ComponentName = Binding->Key.ToString();

					FObjectProperty* ComponentProp = nullptr;
					for (TFieldIterator<FObjectProperty> It(OriginalClass); It; ++It)
					{
						if (It->GetFName() == FName(*ComponentName))
						{
							ComponentProp = *It;
							break;
						}
					}

					if (!ComponentProp && !bStrippedPrefix)
					{
						FString TrimmedName = OriginalComponentName;
						int32 UnderPos;
						while (!ComponentProp && TrimmedName.FindChar(TEXT('_'), UnderPos))
						{
							TrimmedName = TrimmedName.Mid(UnderPos + 1);
							for (TFieldIterator<FObjectProperty> It(OriginalClass); It; ++It)
							{
								if (It->GetFName() == FName(*TrimmedName))
								{
									ComponentProp = *It;
									ComponentName = TrimmedName;
									break;
								}
							}
						}
					}

					if (!ComponentProp)
					{
						UObject* CDO = OriginalClass->GetDefaultObject();
						if (CDO)
						{
							for (TFieldIterator<FObjectProperty> It(OriginalClass); It; ++It)
							{
								UObject* DefaultComp = It->GetObjectPropertyValue(
									It->ContainerPtrToValuePtr<void>(CDO));
								if (DefaultComp && DefaultComp->GetName() == ComponentName)
								{
									ComponentProp = *It;
									UE_LOG(LogGraphBuilder, Log, TEXT("BndEvt component '%s' matched via CDO subobject name -> property '%s'"),
										*ComponentName, *It->GetName());
									ComponentName = It->GetName();
									break;
								}
							}
						}
					}

					if (!ComponentProp)
					{

						TArray<FObjectProperty*> ClassMatches;
						FString BaseClassName; 

						for (TFieldIterator<FObjectProperty> It(OriginalClass); It; ++It)
						{
							if (!It->PropertyClass) continue;
							FString ClassName = It->PropertyClass->GetName();
							ClassName.RemoveFromEnd(TEXT("Component"));

							if (ComponentName == ClassName)
							{
								BaseClassName = ClassName;
								ClassMatches.Add(*It);
							}
							else if (ComponentName.StartsWith(ClassName) && ComponentName.Len() > ClassName.Len())
							{

								FString Suffix = ComponentName.Mid(ClassName.Len());
								bool bAllDigits = true;
								for (TCHAR Ch : Suffix)
								{
									if (!FChar::IsDigit(Ch)) { bAllDigits = false; break; }
								}
								if (bAllDigits)
								{
									BaseClassName = ClassName;
									ClassMatches.Add(*It);
								}
							}
						}

						if (ClassMatches.Num() > 0)
						{

							int32 InstanceIndex = 0;
							if (ComponentName.Len() > BaseClassName.Len())
							{
								FString Suffix = ComponentName.Mid(BaseClassName.Len());
								InstanceIndex = FCString::Atoi(*Suffix);
							}

							TArray<FObjectProperty*> AllOfType;
							for (TFieldIterator<FObjectProperty> It(OriginalClass); It; ++It)
							{
								if (It->PropertyClass && It->PropertyClass->GetName().StartsWith(BaseClassName))
								{
									AllOfType.Add(*It);
								}
							}

							if (InstanceIndex < AllOfType.Num())
							{
								ComponentProp = AllOfType[InstanceIndex];
							}
							else if (AllOfType.Num() > 0)
							{
								ComponentProp = AllOfType[0]; 
							}

							if (ComponentProp)
							{
								UE_LOG(LogGraphBuilder, Log, TEXT("BndEvt component '%s' matched via class name '%s' (instance %d) -> property '%s'"),
									*ComponentName, *BaseClassName, InstanceIndex, *ComponentProp->GetName());
								ComponentName = ComponentProp->GetName();
							}
						}
					}

					if (ComponentProp && ComponentProp->PropertyClass)
					{
						UClass* ComponentClass = ComponentProp->PropertyClass;

						TArray<FMulticastDelegateProperty*> MatchingDelegates;
						for (TFieldIterator<FProperty> It(ComponentClass); It; ++It)
						{
							FMulticastDelegateProperty* MCDP = CastField<FMulticastDelegateProperty>(*It);
							if (MCDP && MCDP->SignatureFunction)
							{
								if (MCDP->SignatureFunction->GetName() == DelegateSigFuncName)
								{
									MatchingDelegates.Add(MCDP);
								}
							}
						}

						FMulticastDelegateProperty* DelegateProp = Binding ? FindFProperty<FMulticastDelegateProperty>(ComponentClass, Binding->Value) : nullptr;
						if (!Binding && MatchingDelegates.Num() > 0)
						{
							FString SignatureKey = ComponentName + TEXT("::") + DelegateSigFuncName;
							int32& Counter = BoundEventSignatureCounters.FindOrAdd(SignatureKey);
							int32 Index = Counter % MatchingDelegates.Num();
							DelegateProp = MatchingDelegates[Index];
							Counter++;

							if (MatchingDelegates.Num() > 1)
							{
								UE_LOG(LogGraphBuilder, Log, TEXT("Multiple delegates (%d) share signature '%s' on component '%s' — assigning BndEvt to delegate[%d]='%s'"),
									MatchingDelegates.Num(), *DelegateSigFuncName, *ComponentName, Index, *DelegateProp->GetName());
							}
						}

						if (!DelegateProp)
						{
							FString DelegateSigName = DelegateSigFuncName;
							DelegateSigName.RemoveFromEnd(TEXT("__DelegateSignature"));
							DelegateSigName.RemoveFromEnd(TEXT("Signature"));
							FString DelegatePropName = TEXT("On") + DelegateSigName;

							for (TFieldIterator<FProperty> It(ComponentClass); It; ++It)
							{
								if (It->GetFName() == FName(*DelegatePropName))
								{
									DelegateProp = CastField<FMulticastDelegateProperty>(*It);
									break;
								}

								if (It->GetFName() == FName(*DelegateSigName))
								{
									DelegateProp = CastField<FMulticastDelegateProperty>(*It);
									break;
								}
							}
						}

						if (DelegateProp)
						{
							UK2Node_ComponentBoundEvent* BoundEvent = NewObject<UK2Node_ComponentBoundEvent>(EventGraph);
							BoundEvent->CreateNewGuid();
							BoundEvent->InitializeComponentBoundEventParams(ComponentProp, DelegateProp);
							BoundEvent->NodePosX = 0;
							BoundEvent->NodePosY = EventY;
							BoundEvent->AllocateDefaultPins();
							EventGraph->AddNode(BoundEvent, false, false);
							EventNode = BoundEvent;
							EventExecOut = FindExecPin(BoundEvent, EGPD_Output);
							bHandledAsComponentBoundEvent = true;

							UE_LOG(LogGraphBuilder, Log, TEXT("Created ComponentBoundEvent: component=%s delegate=%s on %s"),
								*ComponentName, *DelegateProp->GetName(), *ComponentClass->GetName());
						}
						else
						{
							Warn(FString::Printf(TEXT("Could not find delegate property for component bound event '%s' on %s — falling back to CustomEvent"),
								*EventNameStr, *ComponentClass->GetName()));
						}
					}
					else
					{
						Warn(FString::Printf(TEXT("Could not find component property '%s' on %s for bound event"),
							*ComponentName, *OriginalClass->GetName()));
					}
				}
			}
		}

		if (!bHandledAsComponentBoundEvent)
		{

		if (bBuildingChildClass && OriginalClass)
		{

			if (OriginalClass->GetSuperClass())
			{
				ParentFunc = OriginalClass->GetSuperClass()->FindFunctionByName(EventName);
				if (ParentFunc && ParentFunc->HasAnyFunctionFlags(FUNC_BlueprintEvent))
				{
					bIsOverrideEvent = true;
				}
			}

			if (!bIsOverrideEvent)
			{
				ParentFunc = OriginalClass->FindFunctionByName(EventName);
				if (ParentFunc)
				{
					bIsOverrideEvent = true;
				}
			}
		}
		else if (OriginalClass && OriginalClass->GetSuperClass())
		{
			ParentFunc = OriginalClass->GetSuperClass()->FindFunctionByName(EventName);
			if (ParentFunc && ParentFunc->HasAnyFunctionFlags(FUNC_BlueprintEvent))
			{
				bIsOverrideEvent = true;
			}
		}

		if (!bIsOverrideEvent && OriginalClass)
		{
			for (const FImplementedInterface& Iface : OriginalClass->Interfaces)
			{
				if (Iface.Class)
				{
					UFunction* IfaceFunc = Iface.Class->FindFunctionByName(EventName);
					if (IfaceFunc)
					{
						ParentFunc = IfaceFunc;
						bIsOverrideEvent = true;
						break;
					}
				}
			}
		}

		if (!bIsOverrideEvent && EventName.ToString().StartsWith(TEXT("Receive")))
		{
			bIsOverrideEvent = true;
		}

		if (bIsOverrideEvent && (StubFunction || (bBuildingChildClass && ParentFunc)))
		{

			UK2Node_Event* Event = NewObject<UK2Node_Event>(EventGraph);
			Event->CreateNewGuid();

			if (ParentFunc)
			{

				UFunction* RootFunc = ParentFunc;
				while (UFunction* SuperFunc = RootFunc->GetSuperFunction())
				{
					RootFunc = SuperFunc;
				}
				UClass* DeclaringClass = RootFunc->GetOwnerClass();

				if (UBlueprint* OwnerBP = Cast<UBlueprint>(DeclaringClass->ClassGeneratedBy))
				{
					if (OwnerBP->GeneratedClass)
					{
						DeclaringClass = OwnerBP->GeneratedClass;
					}
				}

				Event->EventReference.SetExternalMember(EventName, DeclaringClass);
			}
			else if (OriginalClass->GetSuperClass())
			{

				UClass* FallbackClass = OriginalClass->GetSuperClass();
				while (FallbackClass && FallbackClass->ClassGeneratedBy)
				{
					FallbackClass = FallbackClass->GetSuperClass();
				}
				if (!FallbackClass)
				{
					FallbackClass = OriginalClass->GetSuperClass();
				}
				Event->EventReference.SetExternalMember(EventName, FallbackClass);
			}

			Event->bOverrideFunction = true;
			Event->NodePosX = 0;
			Event->NodePosY = EventY;
			Event->AllocateDefaultPins();
			EventGraph->AddNode(Event, false, false);
			EventNode = Event;
			EventExecOut = FindExecPin(Event, EGPD_Output);
		}
		else
		{

			UK2Node_CustomEvent* CustomEvent = NewObject<UK2Node_CustomEvent>(EventGraph);
			CustomEvent->CreateNewGuid();
			CustomEvent->CustomFunctionName = EventName;
			CustomEvent->NodePosX = 0;
			CustomEvent->NodePosY = EventY;
			CustomEvent->AllocateDefaultPins();

			if (StubFunction)
			{
				for (TFieldIterator<FProperty> ParamIt(StubFunction); ParamIt; ++ParamIt)
				{
					FProperty* Param = *ParamIt;
					if (!Param->HasAnyPropertyFlags(CPF_Parm) || Param->HasAnyPropertyFlags(CPF_ReturnParm))
						continue;

					FEdGraphPinType PinType;
					GetDefault<UEdGraphSchema_K2>()->ConvertPropertyToPinType(Param, PinType);
					CustomEvent->CreateUserDefinedPin(Param->GetFName(), PinType, EGPD_Output);
				}
			}

			EventGraph->AddNode(CustomEvent, false, false);
			EventNode = CustomEvent;
			EventExecOut = FindExecPin(CustomEvent, EGPD_Output);
		}
		} 
		} 

		UE_LOG(LogGraphBuilder, Log, TEXT("Event: %s (statements %d-%d, offset 0x%04X)"),
			*EventName.ToString(), StartStatementIdx, EndStatementIdx, EntryOffset);

		ClearLocalVarMap();
		UFunction* ParameterFunc = StubFunction ? StubFunction : ParentFunc;
		if (ParameterFunc && EventNode)
		{
			for (TFieldIterator<FProperty> ParamIt(ParameterFunc); ParamIt; ++ParamIt)
			{
				FProperty* Param = *ParamIt;
				if (!Param->HasAnyPropertyFlags(CPF_Parm) || Param->HasAnyPropertyFlags(CPF_ReturnParm))
					continue;

				FName ParamName = Param->GetFName();
				for (UEdGraphPin* Pin : EventNode->Pins)
				{
					if (Pin->Direction == EGPD_Output && Pin->PinName == ParamName)
					{
						LocalVarToPinMap.Add(ParamName, Pin);
						break;
					}
				}
			}
		}

		for (const FCASDecompiledFunction& Stub : EventStubs)
			if (FName(*Stub.FunctionName) == EventName)
				for (const auto& Statement : Stub.Statements)
					if (Statement && Statement->Token == EX_LetValueOnPersistentFrame && Statement->PropertyRef && Statement->Children.Num() == 1 && Statement->Children[0] && Statement->Children[0]->PropertyRef)
						if (UEdGraphPin** ParameterPin = LocalVarToPinMap.Find(Statement->Children[0]->PropertyRef->GetFName()))
						{
							UEdGraphPin* Pin = *ParameterPin;
							LocalVarToPinMap.Add(Statement->PropertyRef->GetFName(), Pin);
							PersistentFrameVarMap.Add(Statement->PropertyRef->GetFName(), Pin);
						}
		if (bIsOverrideEvent && ParentFunc && EventNode)
		{
			for (TFieldIterator<FProperty> ParamIt(ParentFunc); ParamIt; ++ParamIt)
			{
				FProperty* Param = *ParamIt;
				if (!Param->HasAnyPropertyFlags(CPF_Parm) || Param->HasAnyPropertyFlags(CPF_ReturnParm))
					continue;

				FName ParamName = Param->GetFName();

				if (LocalVarToPinMap.Contains(ParamName))
					continue;

				for (UEdGraphPin* Pin : EventNode->Pins)
				{
					if (Pin->Direction == EGPD_Output && Pin->PinName == ParamName)
					{
						LocalVarToPinMap.Add(ParamName, Pin);
						UE_LOG(LogGraphBuilder, Log, TEXT("Registered event param '%s' from parent function %s"),
							*ParamName.ToString(), *ParentFunc->GetName());
						break;
					}
				}
			}
		}

		int32 EventEntryStmtIdx = -1;
		{
			int32* EntryIdxPtr = OffsetToStmtIdx.Find(EntryOffset);
			if (EntryIdxPtr)
			{
				EventEntryStmtIdx = *EntryIdxPtr;
			}
		}

		if (EventEntryStmtIdx >= 0 && EventEntryStmtIdx > StartStatementIdx)
		{
			UE_LOG(LogGraphBuilder, Log, TEXT("Event '%s': backward-jump detected — reordering processing. Entry stmt %d, range [%d, %d)"),
				*EventName.ToString(), EventEntryStmtIdx, StartStatementIdx, EndStatementIdx);
		}

		EmitStatements(EventGraph, UbergraphFunc.Statements,
			StartStatementIdx, EndStatementIdx,
			NodeSpacingX, EventY, EventExecOut,
			pReachable, EventEntryStmtIdx);

		EventY += NodeSpacingY * 4; 
	}

    for (const auto& Wire : DeferredEventParameterWires)
    {
        if (auto Source = PersistentFrameVarMap.Find(Wire.Value); Source && *Source) ForceConnect(*Source, Wire.Key);
        else Warn(FString::Printf(TEXT("Cannot resolve persistent event parameter %s"), *Wire.Value.ToString()));
    }
    DeferredEventParameterWires.Reset();

	if (SortedEntries.Num() == 0 && UbergraphFunc.Statements.Num() > 0)
	{
		Warn(TEXT("Ubergraph has statements but no identified event entry points. Emitting as linear graph."));
		EmitStatements(EventGraph, UbergraphFunc.Statements,
			0, UbergraphFunc.Statements.Num(),
			0, 0, nullptr);
	}
}

void FCASGraphBuilder::BuildFunctionGraph(UBlueprint* BP, const FCASDecompiledFunction& Func, UClass* OriginalClass)
{
	CurrentFunctionSignature = Func.OriginalFunction;
	bBuiltReturnNodes = false;

	if (Func.FunctionName.StartsWith(TEXT("ExecuteUbergraph_")))
		return;

	if (Func.FunctionName.Contains(TEXT("__UpdateFunc")) || Func.FunctionName.Contains(TEXT("__FinishedFunc")))
		return;

	FName GraphName(*Func.FunctionName);

	for (UEdGraph* Existing : BP->UbergraphPages)
	{
		for (UEdGraphNode* Node : Existing->Nodes)
		{
			UK2Node_Event* EventNode = Cast<UK2Node_Event>(Node);
			if (EventNode && EventNode->GetFunctionName() == GraphName)
			{
				UE_LOG(LogGraphBuilder, Log, TEXT("Function '%s' already exists as event in ubergraph, skipping"),
					*Func.FunctionName);
				return;
			}
			UK2Node_CustomEvent* CustomNode = Cast<UK2Node_CustomEvent>(Node);
			if (CustomNode && CustomNode->CustomFunctionName == GraphName)
			{
				UE_LOG(LogGraphBuilder, Log, TEXT("Function '%s' already exists as custom event in ubergraph, skipping"),
					*Func.FunctionName);
				return;
			}
		}
	}

	for (int32 i = BP->FunctionGraphs.Num() - 1; i >= 0; --i)
	{
		if (BP->FunctionGraphs[i] && BP->FunctionGraphs[i]->GetFName() == GraphName)
		{
			UE_LOG(LogGraphBuilder, Log, TEXT("Removing existing default function graph '%s' to replace with decompiled version"),
				*Func.FunctionName);
			FBlueprintEditorUtils::RemoveGraph(BP, BP->FunctionGraphs[i], EGraphRemoveFlags::MarkTransient);
			break;
		}
	}

	UE_LOG(LogGraphBuilder, Log, TEXT("Building function graph: %s (%d statements)"),
		*Func.FunctionName, Func.Statements.Num());

	UEdGraph* FuncGraph = FBlueprintEditorUtils::CreateNewGraph(BP,
		GraphName,
		UEdGraph::StaticClass(),
		UEdGraphSchema_K2::StaticClass());

	BP->FunctionGraphs.Add(FuncGraph);

	UK2Node_FunctionEntry* EntryNode = nullptr;
	for (UEdGraphNode* Node : FuncGraph->Nodes)
	{
		EntryNode = Cast<UK2Node_FunctionEntry>(Node);
		if (EntryNode) break;
	}

	if (!EntryNode)
	{

		EntryNode = NewObject<UK2Node_FunctionEntry>(FuncGraph);
		EntryNode->CreateNewGuid();
		EntryNode->NodePosX = 0;
		EntryNode->NodePosY = 0;
		EntryNode->CustomGeneratedFunctionName = GraphName;
		EntryNode->AllocateDefaultPins();
		FuncGraph->AddNode(EntryNode, false, false);
	}

	if (EntryNode->CustomGeneratedFunctionName == NAME_None)
	{
		EntryNode->CustomGeneratedFunctionName = GraphName;
	}

	bool bIsOverride = false;
	if (Func.OriginalFunction && BP->ParentClass)
	{
		UFunction* ParentFunc = BP->ParentClass->FindFunctionByName(GraphName);
		if (ParentFunc)
		{

			bIsOverride = true;
			EntryNode->CustomGeneratedFunctionName = NAME_None;
			EntryNode->FunctionReference.SetExternalMember(GraphName, ParentFunc->GetOwnerClass());
			EntryNode->UserDefinedPins.Empty();
			EntryNode->Pins.Empty();
			EntryNode->AllocateDefaultPins();
		}

	}

	if (!bIsOverride && OriginalClass)
	{
		for (const FImplementedInterface& Iface : OriginalClass->Interfaces)
		{
			if (Iface.Class)
			{
				UFunction* IfaceFunc = Iface.Class->FindFunctionByName(GraphName);
				if (IfaceFunc)
				{
					bIsOverride = true;
					EntryNode->CustomGeneratedFunctionName = NAME_None;
					EntryNode->FunctionReference.SetExternalMember(GraphName, Iface.Class);
					EntryNode->UserDefinedPins.Empty();
					EntryNode->Pins.Empty();
					EntryNode->AllocateDefaultPins();
					UE_LOG(LogGraphBuilder, Log, TEXT("Function '%s' implements interface '%s'"),
						*Func.FunctionName, *Iface.Class->GetName());
					break;
				}
			}
		}
	}

	if (Func.OriginalFunction && !bIsOverride)
	{
		UFunction* OrigFunc = Func.OriginalFunction;
		EntryNode->AddExtraFlags(OrigFunc->FunctionFlags & (FUNC_BlueprintCallable | FUNC_Static | FUNC_Public | FUNC_Protected | FUNC_Private));

		if (OrigFunc->HasAnyFunctionFlags(FUNC_BlueprintPure))
		{

		}

		bool bHasReturnValue = false;
		for (TFieldIterator<FProperty> It(OrigFunc); It; ++It)
		{
			if (It->HasAnyPropertyFlags(CPF_ReturnParm | CPF_OutParm))
			{
				bHasReturnValue = true;
				break;
			}
		}

		for (TFieldIterator<FProperty> It(OrigFunc); It; ++It)
		{
			FProperty* Param = *It;
			if (!Param->HasAnyPropertyFlags(CPF_Parm)) continue;
			if (Param->HasAnyPropertyFlags(CPF_ReturnParm)) continue;
			if (BP->BlueprintType == BPTYPE_FunctionLibrary && Param->GetFName() == TEXT("__WorldContext")) continue;

			FEdGraphPinType PinType;
			GetDefault<UEdGraphSchema_K2>()->ConvertPropertyToPinType(Param, PinType);

			if (Param->HasAnyPropertyFlags(CPF_OutParm) && !Param->HasAnyPropertyFlags(CPF_ReferenceParm))
			{

			}
			else
			{
				EntryNode->CreateUserDefinedPin(Param->GetFName(), PinType, EGPD_Output, false);
			}
		}

		if (EventStubParamNames.Num() > 0)
		{
			TSet<FName> AlreadyAdded;
			for (TFieldIterator<FProperty> It(OrigFunc); It; ++It)
			{
				if (It->HasAnyPropertyFlags(CPF_Parm) && !It->HasAnyPropertyFlags(CPF_ReturnParm))
				{
					AlreadyAdded.Add(It->GetFName());
				}
			}

			for (TFieldIterator<FProperty> It(OrigFunc); It; ++It)
			{
				FProperty* Local = *It;
				FName LocalName = Local->GetFName();

				if (AlreadyAdded.Contains(LocalName)) continue;

				FString NameStr = LocalName.ToString();
				if (NameStr.StartsWith(TEXT("Temp_")) || NameStr.Contains(TEXT("K2Node_")) ||
					NameStr.Contains(TEXT("CallFunc_"))) continue;

				if (EventStubParamNames.Contains(LocalName))
				{
					FEdGraphPinType PinType;
					GetDefault<UEdGraphSchema_K2>()->ConvertPropertyToPinType(Local, PinType);
					EntryNode->CreateUserDefinedPin(LocalName, PinType, EGPD_Output);
					UE_LOG(LogGraphBuilder, Log, TEXT("  Added persistent frame parameter '%s' from event stub"),
						*LocalName.ToString());
				}
			}
		}
	}

	EntryNode->NodePosX = 0;
	EntryNode->NodePosY = 0;
	EntryNode->ReconstructNode();

	UEdGraphPin* EntryExecOut = FindExecPin(EntryNode, EGPD_Output);

	ClearLocalVarMap();
	PersistentFrameVarMap.Reset();
	if (Func.OriginalFunction)
	{
		for (TFieldIterator<FProperty> ParamIt(Func.OriginalFunction); ParamIt; ++ParamIt)
		{
			FProperty* Param = *ParamIt;
			if (!Param->HasAnyPropertyFlags(CPF_Parm) || Param->HasAnyPropertyFlags(CPF_ReturnParm))
				continue;
			if (Param->HasAnyPropertyFlags(CPF_OutParm) && !Param->HasAnyPropertyFlags(CPF_ReferenceParm))
				continue;

			FName ParamName = Param->GetFName();
			for (UEdGraphPin* Pin : EntryNode->Pins)
			{
				if (Pin->Direction == EGPD_Output && Pin->PinName == ParamName)
				{
					LocalVarToPinMap.Add(ParamName, Pin);
					break;
				}
			}
		}

		for (UEdGraphPin* Pin : EntryNode->Pins)
		{
			if (Pin->Direction == EGPD_Output &&
				Pin->PinType.PinCategory != UEdGraphSchema_K2::PC_Exec &&
				!LocalVarToPinMap.Contains(Pin->PinName))
			{
				if (EventStubParamNames.Contains(Pin->PinName))
				{
					LocalVarToPinMap.Add(Pin->PinName, Pin);
				}
			}
		}
	}

	EmitStatements(FuncGraph, Func.Statements,
		0, Func.Statements.Num(),
		NodeSpacingX, 0, EntryExecOut);

	if (Func.OriginalFunction)
	{
		bool bHasReturnValue = false;
		bool bHasOutParams = false;
		FProperty* ReturnProp = nullptr;

		for (TFieldIterator<FProperty> It(Func.OriginalFunction); It; ++It)
		{
			FProperty* Param = *It;
			if (!Param->HasAnyPropertyFlags(CPF_Parm)) continue;
			if (Param->HasAnyPropertyFlags(CPF_ReturnParm))
			{
				bHasReturnValue = true;
				ReturnProp = Param;
			}
			else if (Param->HasAnyPropertyFlags(CPF_OutParm) && !Param->HasAnyPropertyFlags(CPF_ReferenceParm))
			{
				bHasOutParams = true;
			}
		}

		if ((bHasReturnValue || bHasOutParams) && !bBuiltReturnNodes)
		{
			UK2Node_FunctionResult* ResultNode = NewObject<UK2Node_FunctionResult>(FuncGraph);
			ResultNode->CreateNewGuid();

			if (bIsOverride && BP->ParentClass)
			{

				UFunction* ParFunc = BP->ParentClass->FindFunctionByName(GraphName);
				if (ParFunc)
				{
					ResultNode->FunctionReference.SetExternalMember(GraphName, ParFunc->GetOwnerClass());
				}
				else
				{
					ResultNode->FunctionReference.SetSelfMember(GraphName);
				}
			}
			else
			{

				if (Func.OriginalFunction && Func.OriginalFunction->GetOwnerClass())
				{
					ResultNode->FunctionReference.SetSelfMember(GraphName);
				}
				else
				{
					ResultNode->FunctionReference.SetSelfMember(GraphName);
				}
			}

			ResultNode->NodePosX = NodeSpacingX + Func.Statements.Num() * NodeSpacingX;
			ResultNode->NodePosY = 0;
			ResultNode->AllocateDefaultPins();

			if (!bIsOverride)
			{
				for (TFieldIterator<FProperty> It(Func.OriginalFunction); It; ++It)
				{
					FProperty* Param = *It;
					if (!Param->HasAnyPropertyFlags(CPF_Parm)) continue;
					if (Param->HasAnyPropertyFlags(CPF_ReturnParm) || Param->HasAnyPropertyFlags(CPF_OutParm))
					{
						FEdGraphPinType PinType;
						GetDefault<UEdGraphSchema_K2>()->ConvertPropertyToPinType(Param, PinType);
						FName PinName = Param->HasAnyPropertyFlags(CPF_ReturnParm)
							? UEdGraphSchema_K2::PN_ReturnValue
							: Param->GetFName();
						ResultNode->CreateUserDefinedPin(PinName, PinType, EGPD_Input);
					}
				}
			}
			ResultNode->ReconstructNode();

			FuncGraph->AddNode(ResultNode, false, false);

			for (UEdGraphPin* Pin : ResultNode->Pins)
			{
				if (Pin->Direction != EGPD_Input ||
					Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Exec ||
					Pin->LinkedTo.Num() > 0)
				{
					continue;  
				}

				FName PinName = Pin->PinName;
				UEdGraphPin** FoundPin = LocalVarToPinMap.Find(PinName);

				if (!FoundPin)
				{
					FString PinStr = PinName.ToString().Replace(TEXT(" "), TEXT(""));
					for (auto& Pair : LocalVarToPinMap)
					{
						FString VarStr = Pair.Key.ToString().Replace(TEXT(" "), TEXT(""));
						if (VarStr.Equals(PinStr, ESearchCase::IgnoreCase))
						{
							FoundPin = &Pair.Value;
							break;
						}
					}
				}

				if (FoundPin && *FoundPin)
				{
					ForceConnect(*FoundPin, Pin);
				}
				else
				{

					TSharedPtr<FCASDecompiledExpr>* LitExpr = OutParamLiteralMap.Find(PinName);

					if (!LitExpr)
					{
						FString PinStr = PinName.ToString().Replace(TEXT(" "), TEXT(""));
						for (auto& Pair : OutParamLiteralMap)
						{
							FString VarStr = Pair.Key.ToString().Replace(TEXT(" "), TEXT(""));
							if (VarStr.Equals(PinStr, ESearchCase::IgnoreCase))
							{
								LitExpr = &Pair.Value;
								break;
							}
						}
					}

					if (LitExpr && LitExpr->IsValid())
					{
						TSharedPtr<FCASDecompiledExpr> Lit = *LitExpr;

						bool bEnumResolved = false;
						if ((Lit->Token == EX_ByteConst || Lit->Token == EX_IntConst ||
							Lit->Token == EX_IntConstByte || Lit->Token == EX_IntZero || Lit->Token == EX_IntOne) &&
							Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Byte &&
							Pin->PinType.PinSubCategoryObject.IsValid())
						{
							UEnum* PinEnum = Cast<UEnum>(Pin->PinType.PinSubCategoryObject.Get());
							if (PinEnum)
							{
								int64 Val = (Lit->Token == EX_ByteConst) ? Lit->ByteValue : Lit->IntValue;
								FString EnumName = PinEnum->GetNameStringByValue(Val);
								if (!UE427IsEmpty(EnumName))
								{
									Pin->DefaultValue = EnumName;
									bEnumResolved = true;
									UE_LOG(LogGraphBuilder, Log, TEXT("FunctionResult pin '%s' ← enum '%s' (val=%lld from %s)"),
										*PinName.ToString(), *EnumName, Val, *PinEnum->GetName());
								}
							}
						}
						if (!bEnumResolved)
						{
							if (Lit->Token == EX_ObjectConst && Lit->ObjectRef)
							{
								Pin->DefaultObject = Lit->ObjectRef;
							}
							else if (Lit->Token == EX_NoObject)
							{
								Pin->DefaultValue = TEXT("");
							}
							else
							{
								Pin->DefaultValue = Lit->GetLiteralAsString();
							}
							UE_LOG(LogGraphBuilder, Log, TEXT("FunctionResult pin '%s' ← literal default '%s'"),
								*PinName.ToString(), *Pin->DefaultValue);
						}

						DeferredPinDefaults.Add({ResultNode, Pin->PinName, Pin->DefaultValue, Pin->DefaultObject});
					}
				}
			}

			UEdGraphPin* ResultExecIn = FindExecPin(ResultNode, EGPD_Input);
			if (ResultExecIn)
			{
				for (UEdGraphPin* RetExecOut : ReturnExecSources)
				{
					if (RetExecOut && RetExecOut->LinkedTo.Num() == 0)
					{
						TryConnect(RetExecOut, ResultExecIn);
					}
				}

				if (ResultExecIn->LinkedTo.Num() == 0 && LastFunctionExecOut)
				{
					TryConnect(LastFunctionExecOut, ResultExecIn);
				}
				if (ResultExecIn->LinkedTo.Num() > 0)
				{
					UE_LOG(LogGraphBuilder, Log, TEXT("FunctionResult exec wired from %d source(s)"),
						ResultExecIn->LinkedTo.Num());
				}
				else
				{
					UE_LOG(LogGraphBuilder, Warning, TEXT("FunctionResult exec input has NO connections — function may not return"));
				}
			}
		}

		if (Func.OriginalFunction->HasAnyFunctionFlags(FUNC_Const))
		{
			EntryNode->AddExtraFlags(FUNC_Const);
		}
		if (Func.OriginalFunction->HasAnyFunctionFlags(FUNC_BlueprintPure))
		{
			EntryNode->AddExtraFlags(FUNC_BlueprintPure);
		}
	}
}

static void CollectTempVarReads(TSharedPtr<FCASDecompiledExpr> Expr, TMap<FName, int32>& OutFirstReadIdx, int32 StmtIdx)
{
	if (!Expr) return;

	if ((Expr->Token == EX_LocalVariable || Expr->Token == EX_InstanceVariable) && Expr->PropertyRef)
	{
		FName VarName = Expr->PropertyRef->GetFName();
		FString VarStr = VarName.ToString();
		if (VarStr.StartsWith(TEXT("Temp_")))
		{
			if (!OutFirstReadIdx.Contains(VarName) || StmtIdx < OutFirstReadIdx[VarName])
			{
				OutFirstReadIdx.FindOrAdd(VarName) = StmtIdx;
			}
		}
	}

	for (auto& Child : Expr->Children)
	{
		CollectTempVarReads(Child, OutFirstReadIdx, StmtIdx);
	}

	if (Expr->ContextObject)
	{
		CollectTempVarReads(Expr->ContextObject, OutFirstReadIdx, StmtIdx);
	}
}

void FCASGraphBuilder::EmitStatements(
	UEdGraph* Graph,
	const TArray<TSharedPtr<FCASDecompiledExpr>>& Statements,
	int32 StartIndex, int32 EndIndex,
	int32 BaseX, int32 BaseY,
	UEdGraphPin* IncomingExecPin,
	const TSet<int32>* pReachableStmts,
	int32 EventEntryStmtIdx)
{
	int32 CurrentX = BaseX;
	int32 CurrentY = BaseY;
	UEdGraphPin* PrevExecOut = IncomingExecPin;

	EndIndex = FMath::Min(EndIndex, Statements.Num());

	{
		TMap<FName, int32> TempAssignCounts;
		for (int32 i = StartIndex; i < EndIndex; i++)
		{
			TSharedPtr<FCASDecompiledExpr> Stmt = Statements[i];
			if (!Stmt) continue;
			if (pReachableStmts && !pReachableStmts->Contains(i)) continue;

			if (Stmt->Token == EX_Let || Stmt->Token == EX_LetObj ||
				Stmt->Token == EX_LetBool || Stmt->Token == EX_LetDelegate ||
				Stmt->Token == EX_LetMulticastDelegate || Stmt->Token == EX_LetWeakObjPtr)
			{
				if (Stmt->Children.Num() >= 2 && Stmt->Children[0] && Stmt->Children[0]->PropertyRef)
				{
					uint8 DestToken = Stmt->Children[0]->Token;
					if (DestToken == EX_LocalVariable || DestToken == EX_InstanceVariable ||
						DestToken == EX_DefaultVariable || DestToken == EX_LocalOutVariable)
					{
						FName DestName = Stmt->Children[0]->PropertyRef->GetFName();
						FString DestStr = DestName.ToString();
						if (!DestStr.Contains(TEXT("CallFunc_")) && !DestStr.Contains(TEXT("K2Node_")))
						{
							int32& Count = TempAssignCounts.FindOrAdd(DestName);
							Count++;
						}
					}
				}
			}

			if (Stmt->Token == EX_LetValueOnPersistentFrame && Stmt->PropertyRef)
			{
				FName DestName = Stmt->PropertyRef->GetFName();
				FString DestStr = DestName.ToString();
				if (!DestStr.Contains(TEXT("CallFunc_")) && !DestStr.Contains(TEXT("K2Node_")))
				{
					int32& Count = TempAssignCounts.FindOrAdd(DestName);
					Count++;
				}
			}
		}
		for (auto& Pair : TempAssignCounts)
		{
			if (Pair.Value > 1)
			{
				MultiAssignedTempVars.Add(Pair.Key);
			}
		}
	}

	{
		TMap<FName, int32> TempFirstReadIdx;
		TMap<FName, int32> TempFirstWriteIdx;

		for (int32 i = StartIndex; i < EndIndex; i++)
		{
			TSharedPtr<FCASDecompiledExpr> Stmt = Statements[i];
			if (!Stmt) continue;
			if (pReachableStmts && !pReachableStmts->Contains(i)) continue;

			bool bIsLetVariant = (Stmt->Token == EX_Let || Stmt->Token == EX_LetObj ||
				Stmt->Token == EX_LetBool || Stmt->Token == EX_LetDelegate ||
				Stmt->Token == EX_LetMulticastDelegate || Stmt->Token == EX_LetWeakObjPtr);

			if (bIsLetVariant)
			{
				if (Stmt->Children.Num() >= 2 && Stmt->Children[0] && Stmt->Children[0]->PropertyRef)
				{

					FName DestName = Stmt->Children[0]->PropertyRef->GetFName();
					if (DestName.ToString().StartsWith(TEXT("Temp_")))
					{
						if (!TempFirstWriteIdx.Contains(DestName))
						{
							TempFirstWriteIdx.Add(DestName, i);
						}
					}

					if (Stmt->Children.Num() > 1)
					{
						CollectTempVarReads(Stmt->Children[1], TempFirstReadIdx, i);
					}
				}
			}
			else if (Stmt->Token == EX_LetValueOnPersistentFrame && Stmt->PropertyRef)
			{

				FName DestName = Stmt->PropertyRef->GetFName();
				if (DestName.ToString().StartsWith(TEXT("Temp_")))
				{
					if (!TempFirstWriteIdx.Contains(DestName))
					{
						TempFirstWriteIdx.Add(DestName, i);
					}
				}

				for (auto& Child : Stmt->Children)
				{
					CollectTempVarReads(Child, TempFirstReadIdx, i);
				}
			}
			else
			{

				for (auto& Child : Stmt->Children)
				{
					CollectTempVarReads(Child, TempFirstReadIdx, i);
				}
				if (Stmt->ContextObject)
				{
					CollectTempVarReads(Stmt->ContextObject, TempFirstReadIdx, i);
				}
			}
		}

		for (auto& Pair : TempFirstReadIdx)
		{
			if (MultiAssignedTempVars.Contains(Pair.Key))
				continue; 

			int32* WriteIdx = TempFirstWriteIdx.Find(Pair.Key);
			if (!WriteIdx || Pair.Value < *WriteIdx)
			{
				MultiAssignedTempVars.Add(Pair.Key);
				UE_LOG(LogGraphBuilder, Log, TEXT("Stateful Temp_ detected (read-before-write): %s (first read at stmt %d, first write at stmt %d)"),
					*Pair.Key.ToString(), Pair.Value, WriteIdx ? *WriteIdx : -1);
			}
		}
	}

	TMap<int32, UEdGraphNode*> OffsetToNode;
	TArray<TPair<UEdGraphPin*, int32>> DeferredExecWires;
	TArray<int32> FlowStack;
	TMap<int32, int32> ResolvedFlowPops;
	if (!pReachableStmts && Statements.Num() > 0)
	{
		TMap<int32, int32> StatementIndices;
		for (int32 Index = 0; Index < Statements.Num(); ++Index)
			if (Statements[Index]) StatementIndices.Add(Statements[Index]->StartOffset, Index);
		struct FFlowState { int32 Index; TArray<int32> Stack; };
		TArray<FFlowState> Pending;
		Pending.Add({0, {}});
		TSet<FString> Visited;
		TSet<int32> Ambiguous;
		while (!UE427IsEmpty(Pending) && Visited.Num() < 100000)
		{
			FFlowState State = Pending.Pop();
			if (!Statements.IsValidIndex(State.Index) || !Statements[State.Index] || State.Stack.Num() > 64) continue;
			FString Key = FString::FromInt(State.Index);
			for (int32 Target : State.Stack) Key += TEXT(":") + FString::FromInt(Target);
			if (Visited.Contains(Key)) continue;
			Visited.Add(Key);
			const auto& Expr = Statements[State.Index];
			auto QueueTarget = [&](int32 Target, const TArray<int32>& Stack)
			{
				if (const int32* Index = StatementIndices.Find(Target)) Pending.Add({*Index, Stack});
			};
			if (Expr->Token == EX_Return || Expr->Token == EX_EndOfScript) continue;
			if (Expr->Token == EX_Jump) { QueueTarget(Expr->JumpTarget, State.Stack); continue; }
			if (Expr->Token == EX_JumpIfNot) QueueTarget(Expr->JumpTarget, State.Stack);
			if (Expr->Token == EX_PushExecutionFlow) State.Stack.Add(Expr->JumpTarget);
			if (Expr->Token == EX_PopExecutionFlow || Expr->Token == EX_PopExecutionFlowIfNot)
			{
				if (!UE427IsEmpty(State.Stack))
				{
					TArray<int32> Popped = State.Stack;
					int32 Target = Popped.Pop();
					if (const int32* Existing = ResolvedFlowPops.Find(Expr->StartOffset); Existing && *Existing != Target) Ambiguous.Add(Expr->StartOffset);
					ResolvedFlowPops.Add(Expr->StartOffset, Target);
					QueueTarget(Target, Popped);
				}
				if (Expr->Token == EX_PopExecutionFlow) continue;
			}
			Pending.Add({State.Index + 1, State.Stack});
		}
		if (!UE427IsEmpty(Pending)) ResolvedFlowPops.Empty();
		for (int32 Offset : Ambiguous) ResolvedFlowPops.Remove(Offset);
	}
	TMap<int32, int32> NextStatementOffset;
	TMap<int32, int32> JumpForwardMap;
	TSet<int32> ReturnOffsets; 
	int32 PrevProcessedOffset = -1;

	TArray<int32> ProcessOrder;
	if (EventEntryStmtIdx >= 0 && EventEntryStmtIdx > StartIndex)
	{

		for (int32 idx = EventEntryStmtIdx; idx < EndIndex; idx++)
			ProcessOrder.Add(idx);

		ProcessOrder.Add(-1);

		for (int32 idx = StartIndex; idx < EventEntryStmtIdx; idx++)
			ProcessOrder.Add(idx);
	}
	else
	{

		for (int32 idx = StartIndex; idx < EndIndex; idx++)
			ProcessOrder.Add(idx);
	}

	TArray<TPair<UEdGraphPin*, int32>> OrphanFlowPops;  
	TMap<int32, int32> PushTargetToAddr;                 

	for (int32 OrderIdx = 0; OrderIdx < ProcessOrder.Num(); OrderIdx++)
	{
		int32 i = ProcessOrder[OrderIdx];

		if (i == -1)
		{
			PrevExecOut = nullptr;
			PrevProcessedOffset = -1;
			continue;
		}

		TSharedPtr<FCASDecompiledExpr> Stmt = Statements[i];
		if (!Stmt) continue;

		if (pReachableStmts && !pReachableStmts->Contains(i))
		{
			continue;
		}

		if (Stmt->Token == EX_EndOfScript || Stmt->Token == EX_Nothing ||
			Stmt->Token == EX_Tracepoint || Stmt->Token == EX_WireTracepoint ||
			Stmt->Token == EX_InstrumentationEvent || Stmt->Token == EX_Breakpoint)
		{
			continue;
		}

		if (PrevProcessedOffset >= 0)
		{
			NextStatementOffset.Add(PrevProcessedOffset, Stmt->StartOffset);
		}
		PrevProcessedOffset = Stmt->StartOffset;

		if (Stmt->Token == EX_Jump)
		{
			JumpForwardMap.Add(Stmt->StartOffset, Stmt->JumpTarget);
			if (PrevExecOut && Stmt->JumpTarget != 0)
			{
				DeferredExecWires.Add(TPair<UEdGraphPin*, int32>(PrevExecOut, Stmt->JumpTarget));
			}
			PrevExecOut = nullptr; 
			PrevProcessedOffset = -1; 
			continue;
		}

		if (Stmt->Token == EX_PushExecutionFlow)
		{
			FlowStack.Push(Stmt->JumpTarget);

			PushTargetToAddr.Add(Stmt->JumpTarget, Stmt->StartOffset);
			continue;
		}

		if (Stmt->Token == EX_PopExecutionFlow)
		{
			if (const int32* Target = ResolvedFlowPops.Find(Stmt->StartOffset))
			{
				if (PrevExecOut) DeferredExecWires.Add(TPair<UEdGraphPin*, int32>(PrevExecOut, *Target));
				if (!UE427IsEmpty(FlowStack)) FlowStack.Pop(false);
			}
			else if (FlowStack.Num() > 0)
			{
				int32 PopTarget = FlowStack.Pop(false);
				if (PrevExecOut)
				{
					DeferredExecWires.Add(TPair<UEdGraphPin*, int32>(PrevExecOut, PopTarget));
				}
			}
			else if (PrevExecOut)
			{

				OrphanFlowPops.Add(TPair<UEdGraphPin*, int32>(PrevExecOut, Stmt->StartOffset));
				UE_LOG(LogGraphBuilder, Log, TEXT("  Orphan PopExecutionFlow at offset %d (FlowStack empty, recorded for post-processing)"),
					Stmt->StartOffset);
			}
			PrevExecOut = nullptr; 
			PrevProcessedOffset = -1; 
			continue;
		}

		if (Stmt->Token == EX_Return)
		{
			if (CurrentFunctionSignature && Graph->GetFName() == CurrentFunctionSignature->GetFName())
			{
				auto Result = NewObject<UK2Node_FunctionResult>(Graph);
				Result->CreateNewGuid();
				for (UEdGraphNode* Node : Graph->Nodes)
					if (auto Entry = Cast<UK2Node_FunctionEntry>(Node)) Result->FunctionReference = Entry->FunctionReference;
				Result->NodePosX = CurrentX;
				Result->NodePosY = CurrentY;
				Graph->AddNode(Result, false, false);
				Result->AllocateDefaultPins();
				for (TFieldIterator<FProperty> It(CurrentFunctionSignature); It; ++It)
				{
					if (!It->HasAnyPropertyFlags(CPF_Parm) || !It->HasAnyPropertyFlags(CPF_ReturnParm | CPF_OutParm) || It->HasAnyPropertyFlags(CPF_ReferenceParm)) continue;
					FEdGraphPinType Type;
					GetDefault<UEdGraphSchema_K2>()->ConvertPropertyToPinType(*It, Type);
					const FName Name = It->GetFName();
					UEdGraphPin* Pin = Result->FindPin(Name, EGPD_Input);
					if (!Pin) Pin = Result->CreateUserDefinedPin(Name, Type, EGPD_Input, false);
					else if (Result->FunctionReference.GetMemberName().IsNone() && !Result->UserDefinedPins.ContainsByPredicate([Name](const TSharedPtr<FUserPinInfo>& Info) { return Info->PinName == Name; }))
					{
						auto Info = MakeShared<FUserPinInfo>();
						Info->PinName = Name;
						Info->PinType = Type;
						Info->DesiredPinDirection = EGPD_Input;
						Result->UserDefinedPins.Add(Info);
					}
					TSharedPtr<FCASDecompiledExpr> Value;
					if (It->HasAnyPropertyFlags(CPF_ReturnParm) && !UE427IsEmpty(Stmt->Children)) Value = Stmt->Children[0];
					else
					{
						Value = MakeShared<FCASDecompiledExpr>();
						Value->Token = EX_LocalOutVariable;
						Value->PropertyRef = *It;
					}
					if (Value && Value->Token != EX_Self && ApplyTempLiteralToPin(Value, Pin)) {}
					else if (Value && Value->Token != EX_Nothing)
					{
						int32 DataX = CurrentX - 200, DataY = CurrentY + DataNodeOffsetY;
						TryConnect(ResolveDataExpr(Graph, Value, DataX, DataY), Pin);
					}
				}
				TryConnect(PrevExecOut, FindExecPin(Result, EGPD_Input));
				OffsetToNode.Add(Stmt->StartOffset, Result);
				bBuiltReturnNodes = true;
				PrevExecOut = nullptr;
				CurrentX += NodeSpacingX;
				continue;
			}
			ReturnOffsets.Add(Stmt->StartOffset);

			if (PrevExecOut)
			{
				ReturnExecSources.AddUnique(PrevExecOut);
			}

			if (Stmt->Children.Num() > 0 && Stmt->Children[0])
			{
				if (Stmt->Children[0]->Token != EX_Nothing)
				{

					TSharedPtr<FCASDecompiledExpr> RetExpr = Stmt->Children[0];
					if (RetExpr->Token == EX_LocalOutVariable && RetExpr->PropertyRef)
					{

					}
					else if (RetExpr->IsLiteral())
					{

						OutParamLiteralMap.Add(UEdGraphSchema_K2::PN_ReturnValue, RetExpr);
						UE_LOG(LogGraphBuilder, Log, TEXT("EX_Return ← literal (token=0x%02X), stored as ReturnValue"),
							(uint8)RetExpr->Token);
					}
					else
					{

						int32 DataX = CurrentX - 200;
						int32 DataY = CurrentY + DataNodeOffsetY;
						UEdGraphPin* RetPin = ResolveDataExpr(Graph, RetExpr, DataX, DataY);
						if (RetPin)
						{
							LocalVarToPinMap.Add(UEdGraphSchema_K2::PN_ReturnValue, RetPin);
						}
					}
				}
			}

			PrevExecOut = nullptr; 
			continue;
		}

		UEdGraphNode* NewNode = CreateNodeForExpr(Graph, Stmt, CurrentX, CurrentY);

		if (NewNode)
		{

			if (!OffsetToNode.Contains(Stmt->StartOffset))
			{
				OffsetToNode.Add(Stmt->StartOffset, NewNode);
			}

			if (PrevExecOut)
			{
				UEdGraphPin* NodeExecIn = FindExecPin(NewNode, EGPD_Input);
				TryConnect(PrevExecOut, NodeExecIn);
			}

			UEdGraphPin* NodeExecOut = nullptr;

			if (Stmt->Token == EX_JumpIfNot)
			{

				NodeExecOut = FindExecPin(NewNode, EGPD_Output, UEdGraphSchema_K2::PN_Then);

				UEdGraphPin* ElsePin = FindExecPin(NewNode, EGPD_Output, UEdGraphSchema_K2::PN_Else);
				if (ElsePin && Stmt->JumpTarget != 0)
				{
					DeferredExecWires.Add(TPair<UEdGraphPin*, int32>(ElsePin, Stmt->JumpTarget));
				}
			}
			else if (Stmt->Token == EX_PopExecutionFlowIfNot)
			{

				NodeExecOut = FindExecPin(NewNode, EGPD_Output, UEdGraphSchema_K2::PN_Then);

				UEdGraphPin* ElsePin = FindExecPin(NewNode, EGPD_Output, UEdGraphSchema_K2::PN_Else);
				if (ElsePin && ResolvedFlowPops.Contains(Stmt->StartOffset))
				{
					DeferredExecWires.Add(TPair<UEdGraphPin*, int32>(ElsePin, ResolvedFlowPops[Stmt->StartOffset]));
				}
				else if (ElsePin && FlowStack.Num() > 0)
				{
					int32 PopTarget = FlowStack.Last();
					DeferredExecWires.Add(TPair<UEdGraphPin*, int32>(ElsePin, PopTarget));
				}
			}
			else
			{

				NodeExecOut = FindExecPin(NewNode, EGPD_Output);
			}

			if (NodeExecOut)
			{
				PrevExecOut = NodeExecOut;
			}

			if (NewNode && PrevExecOut)
			{
				int32 LatentResumeOffset = -1;
				for (const TSharedPtr<FCASDecompiledExpr>& Child : Stmt->Children)
				{
					if (Child && Child->Token == EX_StructConst && Child->StructRef &&
						Child->StructRef->GetName() == TEXT("LatentActionInfo"))
					{
						for (const TSharedPtr<FCASDecompiledExpr>& StructChild : Child->Children)
						{
							if (StructChild && StructChild->Token == EX_SkipOffsetConst)
							{
								LatentResumeOffset = StructChild->JumpTarget;
								break;
							}
						}
						break;
					}
				}

				if (LatentResumeOffset > 0)
				{
					DeferredExecWires.Add(TPair<UEdGraphPin*, int32>(PrevExecOut, LatentResumeOffset));
					PrevExecOut = nullptr;
					PrevProcessedOffset = -1;
					UE_LOG(LogGraphBuilder, Log, TEXT("Latent function at offset %d, wiring Completed → resume offset %d"),
						Stmt->StartOffset, LatentResumeOffset);
				}
			}

			CurrentX += NodeSpacingX;
		}
	}

	if (OrphanFlowPops.Num() > 0 && FlowStack.Num() > 0)
	{
		UE_LOG(LogGraphBuilder, Log, TEXT("Post-processing: %d orphan Pops, %d leftover FlowStack targets"),
			OrphanFlowPops.Num(), FlowStack.Num());

		TArray<int32> LeftoverTargets;
		while (FlowStack.Num() > 0)
		{
			LeftoverTargets.Add(FlowStack.Pop(false));
		}

		for (int32 Target : LeftoverTargets)
		{
			int32* PushAddrPtr = PushTargetToAddr.Find(Target);
			if (!PushAddrPtr) continue;
			int32 PushAddr = *PushAddrPtr;

			int32 BestIdx = -1;
			int32 BestAddr = -1;
			for (int32 j = 0; j < OrphanFlowPops.Num(); j++)
			{
				int32 PopAddr = OrphanFlowPops[j].Value;
				if (PopAddr < PushAddr && PopAddr > BestAddr)
				{
					BestAddr = PopAddr;
					BestIdx = j;
				}
			}

			if (BestIdx >= 0)
			{
				UEdGraphPin* OrphanExecOut = OrphanFlowPops[BestIdx].Key;
				DeferredExecWires.Add(TPair<UEdGraphPin*, int32>(OrphanExecOut, Target));
				UE_LOG(LogGraphBuilder, Log, TEXT("  Matched orphan Pop@%d → target %d (Push@%d)"),
					OrphanFlowPops[BestIdx].Value, Target, PushAddr);
				OrphanFlowPops.RemoveAt(BestIdx);
			}
			else
			{
				UE_LOG(LogGraphBuilder, Warning, TEXT("  No orphan Pop found for leftover target %d (Push@%d)"),
					Target, PushAddr);
			}
		}

		for (const auto& Orphan : OrphanFlowPops)
		{
			UE_LOG(LogGraphBuilder, Log, TEXT("  Unmatched orphan Pop@%d (no corresponding Push target)"),
				Orphan.Value);
		}
	}

	for (const auto& Wire : DeferredExecWires)
	{
		UEdGraphPin* SourcePin = Wire.Key;
		int32 TargetOffset = Wire.Value;

		if (!SourcePin || SourcePin->LinkedTo.Num() > 0) continue;

		if (UEdGraphNode** ExactNode = OffsetToNode.Find(TargetOffset))
		{
			UEdGraphPin* ExecIn = FindExecPin(*ExactNode, EGPD_Input);
			if (ExecIn)
			{
				TryConnect(SourcePin, ExecIn);
				continue;
			}
		}

		TSet<int32> Visited;
		int32 CurrentOffset = TargetOffset;
		bool bConnected = false;

		while (!bConnected && !Visited.Contains(CurrentOffset))
		{
			Visited.Add(CurrentOffset);

			if (UEdGraphNode** NodePtr = OffsetToNode.Find(CurrentOffset))
			{
				UEdGraphPin* ExecIn = FindExecPin(*NodePtr, EGPD_Input);
				if (ExecIn)
				{
					TryConnect(SourcePin, ExecIn);
					bConnected = true;
					break;
				}
			}

			if (int32* JumpTarget = JumpForwardMap.Find(CurrentOffset))
			{
				CurrentOffset = *JumpTarget;
				continue;
			}

			if (int32* NextOffset = NextStatementOffset.Find(CurrentOffset))
			{
				CurrentOffset = *NextOffset;
				continue;
			}

			break; 
		}

		if (!bConnected)
		{
			UEdGraphNode* TargetNode = nullptr;
			int32 BestOffset = INT32_MAX;
			for (const auto& Entry : OffsetToNode)
			{
				if (Entry.Key >= TargetOffset && Entry.Key < BestOffset)
				{
					BestOffset = Entry.Key;
					TargetNode = Entry.Value;
				}
			}

			if (TargetNode)
			{
				UEdGraphPin* TargetExecIn = FindExecPin(TargetNode, EGPD_Input);
				if (TargetExecIn)
				{
					TryConnect(SourcePin, TargetExecIn);
				}
			}
		}
	}

	for (const auto& Wire : DeferredDataWires)
	{
		UEdGraphPin* InputPin = Wire.Key;
		FName VarName = Wire.Value;

		if (!InputPin || InputPin->LinkedTo.Num() > 0) continue; 

		if (UEdGraphPin** FoundPin = LocalVarToPinMap.Find(VarName))
		{
			if (*FoundPin)
			{
				ForceConnect(*FoundPin, InputPin);
				UE_LOG(LogGraphBuilder, Log, TEXT("DeferredDataWire: %s.%s → %s.%s (var=%s)"),
					*(*FoundPin)->GetOwningNode()->GetNodeTitle(ENodeTitleType::ListView).ToString(),
					*(*FoundPin)->PinName.ToString(),
					*InputPin->GetOwningNode()->GetNodeTitle(ENodeTitleType::ListView).ToString(),
					*InputPin->PinName.ToString(),
					*VarName.ToString());
			}
		}
		else if (UEdGraphPin** PersistPin = PersistentFrameVarMap.Find(VarName))
		{
			if (*PersistPin)
			{
				ForceConnect(*PersistPin, InputPin);
				UE_LOG(LogGraphBuilder, Log, TEXT("DeferredDataWire (cross-event): %s.%s → %s.%s (var=%s)"),
					*(*PersistPin)->GetOwningNode()->GetNodeTitle(ENodeTitleType::ListView).ToString(),
					*(*PersistPin)->PinName.ToString(),
					*InputPin->GetOwningNode()->GetNodeTitle(ENodeTitleType::ListView).ToString(),
					*InputPin->PinName.ToString(),
					*VarName.ToString());
			}
		}
		else if (VarName.ToString().StartsWith(TEXT("K2Node_CustomEvent_")) || VarName.ToString().StartsWith(TEXT("K2Node_Event_")))
		{
			DeferredEventParameterWires.Emplace(InputPin, VarName);
		}
		else
		{
			UE_LOG(LogGraphBuilder, Warning, TEXT("DeferredDataWire UNRESOLVED: %s.%s needs var '%s' (not in LocalVarToPinMap)"),
				*InputPin->GetOwningNode()->GetNodeTitle(ENodeTitleType::ListView).ToString(),
				*InputPin->PinName.ToString(),
				*VarName.ToString());
		}
	}

	for (const auto& Wire : DeferredExecWires)
	{
		UEdGraphPin* SourcePin = Wire.Key;
		int32 TargetOffset = Wire.Value;

		if (!SourcePin || SourcePin->LinkedTo.Num() > 0) continue; 

		TSet<int32> Visited;
		int32 CurrentOffset = TargetOffset;
		while (!Visited.Contains(CurrentOffset))
		{
			Visited.Add(CurrentOffset);
			if (ReturnOffsets.Contains(CurrentOffset))
			{
				ReturnExecSources.AddUnique(SourcePin);
				break;
			}
			if (int32* JumpTarget = JumpForwardMap.Find(CurrentOffset))
			{
				CurrentOffset = *JumpTarget;
				continue;
			}
			if (int32* NextOffset = NextStatementOffset.Find(CurrentOffset))
			{
				CurrentOffset = *NextOffset;
				continue;
			}
			break;
		}
	}

	LastFunctionExecOut = PrevExecOut;
}

UEdGraphNode* FCASGraphBuilder::CreateNodeForExpr(
	UEdGraph* Graph,
	TSharedPtr<FCASDecompiledExpr> Expr,
	int32& X, int32& Y)
{
	if (!Expr || !Graph) return nullptr;

	switch (Expr->Token)
	{

	case EX_FinalFunction:
	case EX_LocalFinalFunction:
	case EX_CallMath:
	{
		UFunction* TargetFunc = Expr->FunctionRef;
		if (!TargetFunc)
		{
			Warn(FString::Printf(TEXT("Null function reference at offset 0x%04X"), Expr->StartOffset));
			return nullptr;
		}

		bool bIsParentCall = false;
		if (!bIsRedirectedVirtualCall && !Expr->ContextObject)
		{
			UClass* FuncOwner = TargetFunc->GetOwnerClass();
			if (FuncOwner && CurrentOriginalClass &&
				FuncOwner != CurrentOriginalClass &&
				(CurrentOriginalClass->IsChildOf(FuncOwner) ||
				 (FuncOwner->HasAnyClassFlags(CLASS_Interface) && CurrentOriginalClass->GetSuperClass()->ImplementsInterface(FuncOwner))))
			{

				FName FuncName = TargetFunc->GetFName();

				if (TargetFunc->HasAnyFunctionFlags(FUNC_BlueprintEvent))
				{
					bIsParentCall = true;
				}
				else
				{

					for (UClass* Check = CurrentOriginalClass; Check; Check = Check->GetSuperClass())
					{
						if (Cast<UBlueprintGeneratedClass>(Check) != nullptr)
							continue; 

						UFunction* NativeFunc = Check->FindFunctionByName(FuncName, EIncludeSuperFlag::ExcludeSuper);
						if (NativeFunc && NativeFunc->HasAnyFunctionFlags(FUNC_BlueprintEvent))
						{
							bIsParentCall = true;
							break;
						}
					}
				}
			}
		}

		if (bIsParentCall)
		{

			UBlueprint* OwnerBP = Cast<UBlueprint>(Graph->GetOuter());
			UClass* ParentClass = OwnerBP ? OwnerBP->ParentClass : nullptr;
			if (!ParentClass && CurrentOriginalClass)
			{
				ParentClass = CurrentOriginalClass->GetSuperClass();
			}

			UFunction* NativeOriginFunc = TargetFunc;
			while (NativeOriginFunc->GetSuperFunction())
			{
				NativeOriginFunc = NativeOriginFunc->GetSuperFunction();
			}

			UK2Node_CallParentFunction* ParentCallNode = NewObject<UK2Node_CallParentFunction>(Graph);
			ParentCallNode->CreateNewGuid();

			ParentCallNode->FunctionReference.SetExternalMember(
				NativeOriginFunc->GetFName(),
				NativeOriginFunc->GetOwnerClass());

			ParentCallNode->NodePosX = X;
			ParentCallNode->NodePosY = Y;
			ParentCallNode->AllocateDefaultPins();
			Graph->AddNode(ParentCallNode, false, false);

			UE_LOG(LogGraphBuilder, Log, TEXT("Created CallParentFunction for %s → parent %s"),
				*TargetFunc->GetName(),
				ParentClass ? *ParentClass->GetName() : TEXT("(native)"));

			int32 DataX = X - 200;
			int32 DataY = Y + DataNodeOffsetY;

			int32 ArgIndex = 0;
			for (const TSharedPtr<FCASDecompiledExpr>& ArgExpr : Expr->Children)
			{
				if (!ArgExpr) { ArgIndex++; continue; }

				FName ParamPinName = GetPinNameForFunctionParam(TargetFunc, ArgIndex);
				if (ParamPinName != NAME_None)
				{
					FString PinNameStr = ParamPinName.ToString();
					if (PinNameStr.Contains(TEXT("LatentInfo")) || PinNameStr == TEXT("LatentActionInfo"))
					{
						ArgIndex++;
						continue;
					}
				}

				if (ParamPinName != NAME_None && ArgExpr->PropertyRef)
				{
					FProperty* ParamProp = nullptr;
					for (TFieldIterator<FProperty> It(TargetFunc); It; ++It)
					{
						if (It->GetFName() == ParamPinName && It->HasAnyPropertyFlags(CPF_Parm))
						{
							ParamProp = *It;
							break;
						}
					}

					if (ParamProp && ParamProp->HasAnyPropertyFlags(CPF_OutParm) &&
						!ParamProp->HasAnyPropertyFlags(CPF_ReturnParm))
					{
						UEdGraphPin* OutPin = FindDataPin(ParentCallNode, EGPD_Output, ParamPinName);
						if (OutPin)
						{
							FName VarName = ArgExpr->PropertyRef->GetFName();
							LocalVarToPinMap.Add(VarName, OutPin);
							ArgIndex++;
							continue;
						}
					}
				}

				UEdGraphPin* DataPin = ResolveDataExpr(Graph, ArgExpr, DataX, DataY);
				if (DataPin && ParamPinName != NAME_None)
				{
					UEdGraphPin* TargetPin = FindDataPin(ParentCallNode, EGPD_Input, ParamPinName);
					if (TargetPin)
					{
						ForceConnect(DataPin, TargetPin);
					}
				}
				ArgIndex++;
				DataY += DataNodeOffsetY;
			}

			X += NodeSpacingX;
			return ParentCallNode;
		}

		bool bIsArrayLibraryFunc = false;
		{
			UClass* FuncClass = TargetFunc->GetOwnerClass();
			if (FuncClass && FuncClass->GetName().Contains(TEXT("KismetArrayLibrary")) && !UE427IsEmpty(TargetFunc->GetMetaData(TEXT("ArrayParm"))))
			{
				bIsArrayLibraryFunc = true;
			}
		}
		UK2Node_CallFunction* CallNode = bIsArrayLibraryFunc
			? NewObject<UK2Node_CallArrayFunction>(Graph)
			: NewObject<UK2Node_CallFunction>(Graph);
		CallNode->CreateNewGuid();

		UClass* FuncOwner = TargetFunc->GetOwnerClass();
		bool bIsBPGCFunc = FuncOwner && Cast<UBlueprintGeneratedClass>(FuncOwner) != nullptr;
		bool bIsSelfBPGCFunc = bIsBPGCFunc && CurrentOriginalClass && CurrentOriginalClass->IsChildOf(FuncOwner);

		if (!bIsSelfBPGCFunc && bIsBPGCFunc && CurrentOriginalClass && !Expr->ContextObject)
		{
			if (CurrentOriginalClass->ImplementsInterface(FuncOwner))
			{
				bIsSelfBPGCFunc = true;
			}
			else
			{

				for (const FImplementedInterface& Iface : CurrentOriginalClass->Interfaces)
				{
					if (Iface.Class == FuncOwner ||
						(Iface.Class && Iface.Class->GetFName() == FuncOwner->GetFName()))
					{
						bIsSelfBPGCFunc = true;
						break;
					}
				}
			}
		}

		CallNode->bIsPureFunc = TargetFunc->HasAnyFunctionFlags(FUNC_BlueprintPure);
		CallNode->bIsConstFunc = TargetFunc->HasAnyFunctionFlags(FUNC_Const);

		if (bIsSelfBPGCFunc)
		{

			CallNode->FunctionReference.SetExternalMember(TargetFunc->GetFName(), FuncOwner);
		}
		else
		{

			UClass* RefClass = FuncOwner ? FuncOwner : CurrentOriginalClass;
			CallNode->FunctionReference.SetExternalMember(TargetFunc->GetFName(), RefClass);
		}

		CallNode->NodePosX = X;
		CallNode->NodePosY = Y;
		CallNode->AllocateDefaultPins();
		FixStructPinDefaults(CallNode);
		Graph->AddNode(CallNode, false, false);

		if (bIsSelfBPGCFunc)
		{

			CallNode->FunctionReference.SetSelfMember(TargetFunc->GetFName());

			UEdGraphPin* SelfPin = FindDataPin(CallNode, EGPD_Input, UEdGraphSchema_K2::PN_Self);
			if (SelfPin)
			{
				CallNode->Pins.Remove(SelfPin);
				SelfPin->MarkPendingKill();
			}
		}

		int32 DataX = X - 200;
		int32 DataY = Y + DataNodeOffsetY;

		int32 ArgIndex = 0;
		for (const TSharedPtr<FCASDecompiledExpr>& ArgExpr : Expr->Children)
		{
			if (!ArgExpr) { ArgIndex++; continue; }

			FName ParamPinName = GetPinNameForFunctionParam(TargetFunc, ArgIndex);
			if (ParamPinName != NAME_None)
			{
				FString PinNameStr = ParamPinName.ToString();
				if (PinNameStr.Contains(TEXT("LatentInfo")) || PinNameStr == TEXT("LatentActionInfo"))
				{
					ArgIndex++;
					continue;
				}
			}

			if (ParamPinName != NAME_None && ArgExpr->PropertyRef)
			{

				FProperty* ParamProp = nullptr;
				for (TFieldIterator<FProperty> It(TargetFunc); It; ++It)
				{
					if (It->GetFName() == ParamPinName && It->HasAnyPropertyFlags(CPF_Parm))
					{
						ParamProp = *It;
						break;
					}
				}

				if (ParamProp && ParamProp->HasAnyPropertyFlags(CPF_OutParm) &&
					!ParamProp->HasAnyPropertyFlags(CPF_ReturnParm))
				{

					UEdGraphPin* OutPin = FindDataPin(CallNode, EGPD_Output, ParamPinName);
					if (OutPin)
					{
						TArray<FString> CustomStructParams;
						TargetFunc->GetMetaData(TEXT("CustomStructureParam")).ParseIntoArray(CustomStructParams, TEXT(","), true);
						if (CustomStructParams.Contains(ParamPinName.ToString()) || bIsArrayLibraryFunc) {
							FEdGraphPinType ActualType;
							if (GetDefault<UEdGraphSchema_K2>()->ConvertPropertyToPinType(ArgExpr->PropertyRef, ActualType)) OutPin->PinType = ActualType;
						}
						FName VarName = ArgExpr->PropertyRef->GetFName();
						LocalVarToPinMap.Add(VarName, OutPin);
						ArgIndex++;
						continue;
					}

				}
			}

			FName PinName = ParamPinName;
			UEdGraphPin* InputPin = nullptr;

			if (PinName != NAME_None)
			{
				InputPin = FindDataPin(CallNode, EGPD_Input, PinName);
			}

			if (!InputPin)
			{
				int32 DataPinIdx = 0;
				for (UEdGraphPin* Pin : CallNode->Pins)
				{
					if (Pin->Direction == EGPD_Input &&
						Pin->PinType.PinCategory != UEdGraphSchema_K2::PC_Exec &&
						Pin->PinName != UEdGraphSchema_K2::PN_Self)
					{
						if (DataPinIdx == ArgIndex)
						{
							InputPin = Pin;
							break;
						}
						DataPinIdx++;
					}
				}
			}

			if (InputPin)
			{

				if (Expr->ContextObject && ArgIndex == 0 &&
					Expr->ContextObject->Token != EX_ObjectConst)
				{

					UEdGraphPin* TargetPin = FindDataPin(CallNode, EGPD_Input, UEdGraphSchema_K2::PN_Self);
					if (TargetPin)
					{
						UEdGraphPin* ContextOutPin = ResolveDataExpr(Graph, Expr->ContextObject, DataX, DataY);

						ForceConnect(ContextOutPin, TargetPin);
						DataY += NodeSpacingY / 2;
					}
				}

				if ((ArgExpr->Token == EX_StructConst || ArgExpr->Token == EX_VectorConst) && InputPin->PinType.bIsReference)
				{
					ForceConnect(ResolveDataExpr(Graph, ArgExpr, DataX, DataY), InputPin);
				}
				else if (ArgExpr->IsLiteral())
				{

					switch (ArgExpr->Token)
					{
					case EX_ObjectConst:
						if (ArgExpr->ObjectRef)
						{

							if (InputPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Class ||
								InputPin->PinType.PinCategory == UEdGraphSchema_K2::PC_SoftClass)
							{
								InputPin->DefaultObject = ArgExpr->ObjectRef;
								InputPin->GetOwningNode()->PinDefaultValueChanged(InputPin);
							}
							else
							{

								InputPin->DefaultObject = ArgExpr->ObjectRef;
							}
						}
						break;

					case EX_NoObject:

						InputPin->DefaultValue = TEXT("");
						break;

					case EX_Self:
					{

						if (!InputPin->bHidden)
						{
							UK2Node_Self* SelfNode = NewObject<UK2Node_Self>(Graph);
							SelfNode->CreateNewGuid();
							SelfNode->NodePosX = DataX;
							SelfNode->NodePosY = DataY;
							SelfNode->AllocateDefaultPins();
							Graph->AddNode(SelfNode, false, false);
							UEdGraphPin* SelfOut = FindFirstDataOutputPin(SelfNode);
							TryConnect(SelfOut, InputPin);
							DataX -= NodeSpacingX / 2;
							DataY += NodeSpacingY / 2;
						}
						break;
					}

					case EX_ByteConst:
					case EX_IntConst:
					case EX_IntConstByte:
					case EX_IntZero:
					case EX_IntOne:
					{

						int64 EnumVal = 0;
						if (ArgExpr->Token == EX_ByteConst)
							EnumVal = ArgExpr->ByteValue;
						else
							EnumVal = ArgExpr->IntValue;
						if (InputPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Byte &&
							InputPin->PinType.PinSubCategoryObject.IsValid())
						{
							UEnum* PinEnum = Cast<UEnum>(InputPin->PinType.PinSubCategoryObject.Get());
							if (PinEnum)
							{
								FString EnumName = PinEnum->GetNameStringByValue(EnumVal);
								if (!UE427IsEmpty(EnumName))
								{
									InputPin->DefaultValue = EnumName;
									break;
								}
							}
						}

						InputPin->DefaultValue = ArgExpr->GetLiteralAsString();
						break;
					}

					default:
					{
						FString DefaultVal = ArgExpr->GetLiteralAsString();

						bool bIsObjectPin = InputPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Object ||
							InputPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Class ||
							InputPin->PinType.PinCategory == UEdGraphSchema_K2::PC_SoftObject ||
							InputPin->PinType.PinCategory == UEdGraphSchema_K2::PC_SoftClass ||
							InputPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Interface;
						if (!UE427IsEmpty(DefaultVal) && !(bIsObjectPin && DefaultVal == TEXT("None")))
						{
							InputPin->DefaultValue = DefaultVal;
						}
						break;
					}
					}
				}
				else
				{

					if (ApplyTempLiteralToPin(ArgExpr, InputPin))
					{

					}
					else
					{

						UEdGraphPin* ArgOutPin = ResolveDataExpr(Graph, ArgExpr, DataX, DataY);
						if (ArgOutPin)
						{
							TryConnect(ArgOutPin, InputPin);
						}
						else if (ArgExpr->PropertyRef &&
							(ArgExpr->Token == EX_LocalVariable || ArgExpr->Token == EX_LocalOutVariable))
						{

							DeferredDataWires.Add(TPair<UEdGraphPin*, FName>(
								InputPin, ArgExpr->PropertyRef->GetFName()));
						}
						DataY += NodeSpacingY / 2;
					}
				}
			}

			ArgIndex++;
		}

		if (Expr->ContextObject && Expr->ContextObject->Token != EX_ObjectConst)
		{
			UEdGraphPin* TargetPin = FindDataPin(CallNode, EGPD_Input, UEdGraphSchema_K2::PN_Self);

			if (!TargetPin)
			{
				TargetPin = FindDataPin(CallNode, EGPD_Input, TEXT("TargetArray"));
			}
			if (!TargetPin)
			{

				for (UEdGraphPin* Pin : CallNode->Pins)
				{
					if (Pin->Direction == EGPD_Input &&
						Pin->PinType.PinCategory != UEdGraphSchema_K2::PC_Exec &&
						Pin->LinkedTo.Num() == 0)
					{
						TargetPin = Pin;
						break;
					}
				}
			}

			if (TargetPin && TargetPin->LinkedTo.Num() == 0)
			{
				UEdGraphPin* ContextOut = ResolveDataExpr(Graph, Expr->ContextObject, DataX, DataY);
				if (!ForceConnect(ContextOut, TargetPin))
				{
					Warn(FString::Printf(TEXT("Failed to wire context object to Target pin of %s"),
						*TargetFunc->GetName()));
				}
			}
		}

		{
			UClass* FuncClass = TargetFunc->GetOwnerClass();
			if (FuncClass && FuncClass->GetName().Contains(TEXT("KismetArrayLibrary")) && !UE427IsEmpty(TargetFunc->GetMetaData(TEXT("ArrayParm"))))
			{
				FEdGraphPinType ElemType;
				bool bFoundElemType = false;

				if (Expr->ContextObject && Expr->ContextObject->PropertyRef)
				{
					FArrayProperty* ArrayProp = CastField<FArrayProperty>(Expr->ContextObject->PropertyRef);
					if (ArrayProp && ArrayProp->Inner)
					{
						GetDefault<UEdGraphSchema_K2>()->ConvertPropertyToPinType(ArrayProp->Inner, ElemType);
						bFoundElemType = true;
					}
				}

				if (!bFoundElemType)
				{
					UEdGraphPin* ArrayPin = FindDataPin(CallNode, EGPD_Input, TEXT("TargetArray"));
					if (!ArrayPin || ArrayPin->LinkedTo.Num() == 0)
					{
						ArrayPin = FindDataPin(CallNode, EGPD_Input, TEXT("Target Array"));
					}
					if (!ArrayPin || ArrayPin->LinkedTo.Num() == 0)
					{
						for (UEdGraphPin* Pin : CallNode->Pins)
						{
							if (Pin->Direction == EGPD_Input &&
								Pin->PinType.PinCategory != UEdGraphSchema_K2::PC_Exec &&
								Pin->LinkedTo.Num() > 0 &&
								(Pin->PinType.ContainerType == EPinContainerType::Array ||
									(Pin->LinkedTo[0]->PinType.ContainerType == EPinContainerType::Array)))
							{
								ArrayPin = Pin;
								break;
							}
						}
					}
					if (ArrayPin && ArrayPin->LinkedTo.Num() > 0)
					{
						FEdGraphPinType ConnectedType = ArrayPin->LinkedTo[0]->PinType;
						if (ConnectedType.ContainerType == EPinContainerType::Array)
						{
							ElemType = ConnectedType;
							ElemType.ContainerType = EPinContainerType::None;
							bFoundElemType = true;
						}
					}
				}

				if (bFoundElemType)
				{
					for (UEdGraphPin* Pin : CallNode->Pins)
					{
						if (Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Wildcard)
						{
							if (Pin->PinType.ContainerType == EPinContainerType::Array)
							{
								Pin->PinType = ElemType;
								Pin->PinType.ContainerType = EPinContainerType::Array;
							}
							else
							{
								Pin->PinType = ElemType;
							}
						}
					}
				}
			}
		}

		return CallNode;
	}

	case EX_CallMulticastDelegate:
	{
		UFunction* SigFunc = Expr->FunctionRef;
		if (!SigFunc)
		{
			Warn(FString::Printf(TEXT("Null signature function for CallMulticastDelegate at offset 0x%04X"), Expr->StartOffset));
			return nullptr;
		}

		if (Expr->Children.Num() < 1)
		{
			Warn(FString::Printf(TEXT("CallMulticastDelegate has no children (expected delegate ref) at offset 0x%04X"), Expr->StartOffset));
			return nullptr;
		}

		TSharedPtr<FCASDecompiledExpr> DelegateExpr = Expr->Children[0];
		FProperty* DelegateProp = nullptr;
		bool bSelfContext = false;
		TSharedPtr<FCASDecompiledExpr> TargetObjectExpr;

		if (DelegateExpr)
		{
			if (DelegateExpr->Token == EX_InstanceVariable && DelegateExpr->PropertyRef)
			{

				DelegateProp = CastField<FProperty>(DelegateExpr->PropertyRef);
				bSelfContext = true;
			}
			else if (DelegateExpr->Token == EX_LocalVariable && DelegateExpr->PropertyRef)
			{

				DelegateProp = CastField<FProperty>(DelegateExpr->PropertyRef);
				bSelfContext = true;
			}
			else if (DelegateExpr->Token == EX_DefaultVariable && DelegateExpr->PropertyRef)
			{

				DelegateProp = CastField<FProperty>(DelegateExpr->PropertyRef);
				bSelfContext = true;
			}
			else if (DelegateExpr->Token == EX_Context || DelegateExpr->Token == EX_Context_FailSilent)
			{

				TargetObjectExpr = DelegateExpr->ContextObject;
				if (DelegateExpr->FieldRef)
				{
					DelegateProp = CastField<FProperty>(DelegateExpr->FieldRef);
				}
				if (!DelegateProp && DelegateExpr->Children.Num() > 0 &&
					DelegateExpr->Children[0] && DelegateExpr->Children[0]->PropertyRef)
				{
					DelegateProp = CastField<FProperty>(DelegateExpr->Children[0]->PropertyRef);
				}
			}

			if (!DelegateProp && DelegateExpr->PropertyRef)
			{
				DelegateProp = CastField<FProperty>(DelegateExpr->PropertyRef);
				bSelfContext = true; 
			}
		}

		if (!DelegateProp && SigFunc)
		{
			FString SigName = SigFunc->GetName();
			if (SigName.RemoveFromEnd(TEXT("__DelegateSignature")))
			{
				UClass* SigOwner = SigFunc->GetOwnerClass();
				if (SigOwner)
				{

					for (TFieldIterator<FProperty> It(SigOwner); It; ++It)
					{
						if (It->GetFName() == FName(*SigName))
						{
							DelegateProp = *It;
							bSelfContext = (CurrentOriginalClass && CurrentOriginalClass->IsChildOf(SigOwner));
							break;
						}
					}
				}

				if (!DelegateProp && CurrentOriginalClass)
				{
					for (TFieldIterator<FProperty> It(CurrentOriginalClass); It; ++It)
					{
						if (It->GetFName() == FName(*SigName))
						{
							DelegateProp = *It;
							bSelfContext = true;
							break;
						}
					}
				}
			}
		}

		if (!DelegateProp)
		{

			Warn(FString::Printf(TEXT("Could not extract delegate property for CallMulticastDelegate '%s' — falling back to CallFunction at offset 0x%04X"),
				*SigFunc->GetName(), Expr->StartOffset));

			Expr->Token = EX_FinalFunction;
			UEdGraphNode* FallbackNode = CreateNodeForExpr(Graph, Expr, X, Y);
			Expr->Token = EX_CallMulticastDelegate; 
			return FallbackNode;
		}

		UClass* OwnerClass = DelegateProp->GetOwnerClass();

		UK2Node_CallDelegate* CallDelegateNode = NewObject<UK2Node_CallDelegate>(Graph);
		CallDelegateNode->CreateNewGuid();
		CallDelegateNode->SetFromProperty(DelegateProp, bSelfContext, OwnerClass);
		CallDelegateNode->NodePosX = X;
		CallDelegateNode->NodePosY = Y;
		CallDelegateNode->AllocateDefaultPins();
		Graph->AddNode(CallDelegateNode, false, false);

		if (!bSelfContext && TargetObjectExpr)
		{
			UEdGraphPin* SelfPin = FindDataPin(CallDelegateNode, EGPD_Input, UEdGraphSchema_K2::PN_Self);
			if (SelfPin)
			{
				int32 DataX = X - 200;
				int32 DataY = Y + DataNodeOffsetY;
				UEdGraphPin* TargetPin = ResolveDataExpr(Graph, TargetObjectExpr, DataX, DataY);
				ForceConnect(TargetPin, SelfPin);
			}
		}

		{
			int32 DataX = X - 200;
			int32 DataY = Y + DataNodeOffsetY;

			for (int32 ArgIndex = 1; ArgIndex < Expr->Children.Num(); ArgIndex++)
			{
				TSharedPtr<FCASDecompiledExpr> ArgExpr = Expr->Children[ArgIndex];
				if (!ArgExpr) continue;

				FName ParamPinName = GetPinNameForFunctionParam(SigFunc, ArgIndex - 1);

				UEdGraphPin* InputPin = nullptr;
				if (ParamPinName != NAME_None)
				{
					InputPin = FindDataPin(CallDelegateNode, EGPD_Input, ParamPinName);
				}

				if (!InputPin)
				{
					int32 DataPinIdx = 0;
					for (UEdGraphPin* Pin : CallDelegateNode->Pins)
					{
						if (Pin->Direction == EGPD_Input &&
							Pin->PinType.PinCategory != UEdGraphSchema_K2::PC_Exec &&
							Pin->PinName != UEdGraphSchema_K2::PN_Self)
						{
							if (DataPinIdx == (ArgIndex - 1))
							{
								InputPin = Pin;
								break;
							}
							DataPinIdx++;
						}
					}
				}

				if (InputPin)
				{
					UEdGraphPin* DataPin = ResolveDataExpr(Graph, ArgExpr, DataX, DataY);
					if (DataPin)
					{
						ForceConnect(DataPin, InputPin);
					}
				}
				DataY += DataNodeOffsetY;
			}
		}

		X += NodeSpacingX;
		return CallDelegateNode;
	}

	case EX_VirtualFunction:
	case EX_LocalVirtualFunction:
	{

		UFunction* ResolvedFunc = Expr->FunctionRef;
		FName FuncName = Expr->NameValue;

		if (Expr->ContextObject)
		{

			TSharedPtr<FCASDecompiledExpr> CtxExpr = Expr->ContextObject;
			while (CtxExpr && !ResolvedFunc)
			{

				if (CtxExpr->FieldRef)
				{
					FProperty* Prop = CastField<FProperty>(CtxExpr->FieldRef);
					if (Prop)
					{
						FObjectPropertyBase* ObjProp = CastField<FObjectPropertyBase>(Prop);
						if (ObjProp && ObjProp->PropertyClass)
						{
							ResolvedFunc = ObjProp->PropertyClass->FindFunctionByName(FuncName);
						}
						if (!ResolvedFunc)
						{
							FInterfaceProperty* IntProp = CastField<FInterfaceProperty>(Prop);
							if (IntProp && IntProp->InterfaceClass)
							{
								ResolvedFunc = IntProp->InterfaceClass->FindFunctionByName(FuncName);
							}
						}
					}
				}

				if (!ResolvedFunc && CtxExpr->Token == EX_InterfaceContext && CtxExpr->Children.Num() > 0)
				{
					TSharedPtr<FCASDecompiledExpr> InnerExpr = CtxExpr->Children[0];
					if (InnerExpr && InnerExpr->PropertyRef)
					{
						FInterfaceProperty* IntProp = CastField<FInterfaceProperty>(InnerExpr->PropertyRef);
						if (IntProp && IntProp->InterfaceClass)
						{
							ResolvedFunc = IntProp->InterfaceClass->FindFunctionByName(FuncName);
						}
						if (!ResolvedFunc)
						{
							FObjectPropertyBase* ObjProp = CastField<FObjectPropertyBase>(InnerExpr->PropertyRef);
							if (ObjProp && ObjProp->PropertyClass)
							{
								ResolvedFunc = ObjProp->PropertyClass->FindFunctionByName(FuncName);
							}
						}
					}
				}

				if (!ResolvedFunc && CtxExpr->PropertyRef)
				{
					FObjectPropertyBase* ObjProp = CastField<FObjectPropertyBase>(CtxExpr->PropertyRef);
					if (ObjProp && ObjProp->PropertyClass)
					{
						ResolvedFunc = ObjProp->PropertyClass->FindFunctionByName(FuncName);
					}
					if (!ResolvedFunc)
					{
						FInterfaceProperty* IntProp = CastField<FInterfaceProperty>(CtxExpr->PropertyRef);
						if (IntProp && IntProp->InterfaceClass)
						{
							ResolvedFunc = IntProp->InterfaceClass->FindFunctionByName(FuncName);
						}
					}
				}

				if (!ResolvedFunc && CtxExpr->FunctionRef)
				{
					UFunction* CtxFunc = CtxExpr->FunctionRef;
					FProperty* RetProp = CtxFunc->GetReturnProperty();
					if (RetProp)
					{
						FObjectPropertyBase* ObjProp = CastField<FObjectPropertyBase>(RetProp);
						if (ObjProp && ObjProp->PropertyClass)
						{
							ResolvedFunc = ObjProp->PropertyClass->FindFunctionByName(FuncName);
						}
					}
				}

				if (!ResolvedFunc && CtxExpr->ObjectRef)
				{
					UClass* ObjClass = CtxExpr->ObjectRef->GetClass();
					if (ObjClass)
					{
						ResolvedFunc = ObjClass->FindFunctionByName(FuncName);
					}
				}

				if (CtxExpr->ContextObject)
				{
					CtxExpr = CtxExpr->ContextObject;
				}
				else if (CtxExpr->Children.Num() > 0)
				{
					CtxExpr = CtxExpr->Children[0];
				}
				else
				{
					break;
				}
			}
		}

		if (!ResolvedFunc && CurrentOriginalClass)
		{
			ResolvedFunc = CurrentOriginalClass->FindFunctionByName(FuncName);
		}

		if (!ResolvedFunc && Graph->GetOuter())
		{
			UBlueprint* BP = Cast<UBlueprint>(Graph->GetOuter());
			if (BP && BP->ParentClass)
			{
				ResolvedFunc = BP->ParentClass->FindFunctionByName(FuncName);
			}
		}

		if (!ResolvedFunc)
		{

			ResolvedFunc = UKismetSystemLibrary::StaticClass()->FindFunctionByName(FuncName);
		}
		if (!ResolvedFunc)
		{
			ResolvedFunc = UKismetMathLibrary::StaticClass()->FindFunctionByName(FuncName);
		}
		if (!ResolvedFunc)
		{

			UClass* GameplayStatics = LoadObject<UClass>(nullptr, TEXT("/Script/Engine.GameplayStatics"));
			if (GameplayStatics)
			{
				ResolvedFunc = GameplayStatics->FindFunctionByName(FuncName);
			}
		}

		if (ResolvedFunc)
		{

			Expr->FunctionRef = ResolvedFunc;
			Expr->Token = EX_FinalFunction; 
			bIsRedirectedVirtualCall = true; 
			UEdGraphNode* Result = CreateNodeForExpr(Graph, Expr, X, Y);
			bIsRedirectedVirtualCall = false;
			Expr->Token = EX_VirtualFunction; 
			return Result;
		}
		else
		{

			Warn(FString::Printf(TEXT("Could not resolve virtual function: %s"), *FuncName.ToString()));

			return nullptr;
		}
	}

	case EX_JumpIfNot:
	case EX_PopExecutionFlowIfNot:
	{
		UK2Node_IfThenElse* BranchNode = NewObject<UK2Node_IfThenElse>(Graph);
		BranchNode->CreateNewGuid();
		BranchNode->NodePosX = X;
		BranchNode->NodePosY = Y;
		BranchNode->AllocateDefaultPins();
		Graph->AddNode(BranchNode, false, false);

		if (Expr->Children.Num() > 0 && Expr->Children[0])
		{
			UEdGraphPin* ConditionPin = FindDataPin(BranchNode, EGPD_Input, UEdGraphSchema_K2::PN_Condition);
			if (ConditionPin)
			{
				int32 CondX = X - 200;
				int32 CondY = Y + DataNodeOffsetY;

				if (Expr->Children[0]->IsLiteral())
				{
					ConditionPin->DefaultValue = Expr->Children[0]->GetLiteralAsString();
				}
				else
				{
					UEdGraphPin* CondOut = ResolveDataExpr(Graph, Expr->Children[0], CondX, CondY);
					TryConnect(CondOut, ConditionPin);
				}
			}
		}

		return BranchNode;
	}

	case EX_Let:
	case EX_LetObj:
	case EX_LetBool:
	case EX_LetDelegate:
	case EX_LetMulticastDelegate:
	case EX_LetWeakObjPtr:
	{

		if (Expr->Children.Num() < 2) return nullptr;

		TSharedPtr<FCASDecompiledExpr> DestExpr = Expr->Children[0];
		TSharedPtr<FCASDecompiledExpr> SourceExpr = Expr->Children[1];

		if (!DestExpr || !SourceExpr) return nullptr;

		if (DestExpr->PropertyRef &&
			(DestExpr->Token == EX_LocalVariable || DestExpr->Token == EX_InstanceVariable ||
				DestExpr->Token == EX_DefaultVariable || DestExpr->Token == EX_LocalOutVariable))
		{

			if (DestExpr->Token == EX_InstanceVariable)
			{
				UK2Node_VariableSet* SetNode = NewObject<UK2Node_VariableSet>(Graph);
				SetNode->CreateNewGuid();
				SetNode->VariableReference.SetSelfMember(DestExpr->PropertyRef->GetFName());
				SetNode->NodePosX = X;
				SetNode->NodePosY = Y;
				SetNode->AllocateDefaultPins();
				Graph->AddNode(SetNode, false, false);

				FName VarPinName = DestExpr->PropertyRef->GetFName();
				UEdGraphPin* ValuePin = FindDataPin(SetNode, EGPD_Input, VarPinName);
				if (!ValuePin)
				{

					for (UEdGraphPin* Pin : SetNode->Pins)
					{
						if (Pin->Direction == EGPD_Input &&
							Pin->PinType.PinCategory != UEdGraphSchema_K2::PC_Exec)
						{
							ValuePin = Pin;
							break;
						}
					}
				}

				if (ValuePin)
				{
					int32 DataX = X - 200;
					int32 DataY = Y + DataNodeOffsetY;
					if (SourceExpr->IsLiteral())
					{
						if (SourceExpr->Token == EX_NoObject)
						{

							ValuePin->DefaultValue = TEXT("");
						}
						else if (SourceExpr->Token == EX_ObjectConst && SourceExpr->ObjectRef)
						{
							ValuePin->DefaultObject = SourceExpr->ObjectRef;
						}
						else
						{

							bool bEnumResolved = false;
							if ((SourceExpr->Token == EX_ByteConst || SourceExpr->Token == EX_IntConst ||
								SourceExpr->Token == EX_IntConstByte || SourceExpr->Token == EX_IntZero || SourceExpr->Token == EX_IntOne) &&
								ValuePin->PinType.PinCategory == UEdGraphSchema_K2::PC_Byte &&
								ValuePin->PinType.PinSubCategoryObject.IsValid())
							{
								UEnum* PinEnum = Cast<UEnum>(ValuePin->PinType.PinSubCategoryObject.Get());
								if (PinEnum)
								{
									int64 Val = (SourceExpr->Token == EX_ByteConst) ? SourceExpr->ByteValue : SourceExpr->IntValue;
									FString EnumName = PinEnum->GetNameStringByValue(Val);
									if (!UE427IsEmpty(EnumName))
									{
										ValuePin->DefaultValue = EnumName;
										bEnumResolved = true;
									}
								}
							}
							if (!bEnumResolved)
							{
								ValuePin->DefaultValue = SourceExpr->GetLiteralAsString();
							}
						}
					}
					else if (ApplyTempLiteralToPin(SourceExpr, ValuePin))
					{

					}
					else
					{
						UEdGraphPin* SourcePin = ResolveDataExpr(Graph, SourceExpr, DataX, DataY);
						ForceConnect(SourcePin, ValuePin);
					}
				}

				return SetNode;
			}
			else
			{

				FName VarName = DestExpr->PropertyRef->GetFName();

				const bool bIsMultiAssigned = MultiAssignedTempVars.Contains(VarName);

				if (bIsMultiAssigned)
				{
					UK2Node_AssignmentStatement* AssignNode = NewObject<UK2Node_AssignmentStatement>(Graph);
					AssignNode->CreateNewGuid();
					AssignNode->NodePosX = X;
					AssignNode->NodePosY = Y;
					AssignNode->AllocateDefaultPins();
					Graph->AddNode(AssignNode, false, false);

					UEdGraphPin* VarPin = AssignNode->GetVariablePin();
					if (VarPin)
					{

						UEdGraphPin* TempVarOutPin = nullptr;
						if (UEdGraphPin** CachedPin = MultiAssignedTempPins.Find(VarName))
						{
							TempVarOutPin = *CachedPin;
						}
						else
						{

							UK2Node_TemporaryVariable* TempNode = NewObject<UK2Node_TemporaryVariable>(Graph);
							TempNode->CreateNewGuid();
							FEdGraphPinType VarType;
							GetDefault<UEdGraphSchema_K2>()->ConvertPropertyToPinType(DestExpr->PropertyRef, VarType);
							TempNode->VariableType = VarType;
							TempNode->NodePosX = X - NodeSpacingX / 2;
							TempNode->NodePosY = Y - 150;
							TempNode->AllocateDefaultPins();
							Graph->AddNode(TempNode, false, false);
							TempVarOutPin = FindFirstDataOutputPin(TempNode);
							if (TempVarOutPin)
							{
								MultiAssignedTempPins.Add(VarName, TempVarOutPin);
							}
						}
						if (TempVarOutPin)
						{
							VarPin->PinType = TempVarOutPin->PinType;
							if (!TryConnect(TempVarOutPin, VarPin))
								Warn(FString::Printf(TEXT("Cannot connect assignment variable %s in %s"), *VarName.ToString(), *Graph->GetName()));

							UEdGraphPin* ValPin = AssignNode->GetValuePin();
							if (ValPin)
							{
								ValPin->PinType = TempVarOutPin->PinType;
							}
						}
					}

					UEdGraphPin* ValPin = AssignNode->GetValuePin();
					UE_LOG(LogGraphBuilder, Log, TEXT("MultiAssign EX_Let: Var=%s SourceToken=0x%02X IsLiteral=%d ValPin=%s PinCat=%s SubCatValid=%d"),
						*VarName.ToString(), (uint8)SourceExpr->Token, SourceExpr->IsLiteral() ? 1 : 0,
						ValPin ? TEXT("valid") : TEXT("NULL"),
						ValPin ? *ValPin->PinType.PinCategory.ToString() : TEXT("N/A"),
						(ValPin && ValPin->PinType.PinSubCategoryObject.IsValid()) ? 1 : 0);
					if (ValPin)
					{
						int32 DataX = X - 200;
						int32 DataY = Y + DataNodeOffsetY;
						if (SourceExpr->IsLiteral())
						{
							if (SourceExpr->Token == EX_NoObject)
							{
								ValPin->DefaultValue = TEXT("");
							}
							else if (SourceExpr->Token == EX_ObjectConst && SourceExpr->ObjectRef)
							{
								ValPin->DefaultObject = SourceExpr->ObjectRef;
							}
							else
							{

								bool bEnumResolved = false;
								if ((SourceExpr->Token == EX_ByteConst || SourceExpr->Token == EX_IntConst ||
									SourceExpr->Token == EX_IntConstByte || SourceExpr->Token == EX_IntZero || SourceExpr->Token == EX_IntOne) &&
									ValPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Byte &&
									ValPin->PinType.PinSubCategoryObject.IsValid())
								{
									UEnum* PinEnum = Cast<UEnum>(ValPin->PinType.PinSubCategoryObject.Get());
									if (PinEnum)
									{
										int64 Val = (SourceExpr->Token == EX_ByteConst) ? SourceExpr->ByteValue : SourceExpr->IntValue;
										FString EnumName = PinEnum->GetNameStringByValue(Val);
										UE_LOG(LogGraphBuilder, Log, TEXT("  EnumResolve: Enum=%s Val=%lld Name='%s'"),
											*PinEnum->GetName(), Val, *EnumName);
										if (!UE427IsEmpty(EnumName))
										{
											ValPin->DefaultValue = EnumName;
											bEnumResolved = true;
										}
									}
								}
								if (!bEnumResolved)
								{
									ValPin->DefaultValue = SourceExpr->GetLiteralAsString();
									UE_LOG(LogGraphBuilder, Log, TEXT("  Fallback literal: '%s'"), *ValPin->DefaultValue);
								}
							}
							UE_LOG(LogGraphBuilder, Log, TEXT("  After literal set: DefaultValue='%s'"), *ValPin->DefaultValue);

							DeferredPinDefaults.Add({AssignNode, ValPin->PinName, ValPin->DefaultValue, ValPin->DefaultObject});
						}
						else if (ApplyTempLiteralToPin(SourceExpr, ValPin))
						{

							UE_LOG(LogGraphBuilder, Log, TEXT("  ApplyTempLiteralToPin succeeded"));
							DeferredPinDefaults.Add({AssignNode, ValPin->PinName, ValPin->DefaultValue, ValPin->DefaultObject});
						}
						else
						{
							UEdGraphPin* SourcePin = ResolveDataExpr(Graph, SourceExpr, DataX, DataY);
							ForceConnect(SourcePin, ValPin);
							UE_LOG(LogGraphBuilder, Log, TEXT("  ResolveDataExpr path, SourcePin=%s"),
								SourcePin ? TEXT("valid") : TEXT("NULL"));
						}
					}

					if (auto Variable = MultiAssignedTempPins.FindRef(VarName))
					{
						AssignNode->GetVariablePin()->PinType = Variable->PinType;
						AssignNode->GetValuePin()->PinType = Variable->PinType;
						Variable->MakeLinkTo(AssignNode->GetVariablePin());
					}
					X += NodeSpacingX;
					return AssignNode;
				}

				if (DestExpr->Token == EX_LocalOutVariable)
				{

					if (SourceExpr->IsLiteral())
					{
						OutParamLiteralMap.Add(VarName, SourceExpr);
						UE_LOG(LogGraphBuilder, Log, TEXT("EX_LocalOutVariable '%s' ← literal (token=0x%02X), stored in OutParamLiteralMap"),
							*VarName.ToString(), (uint8)SourceExpr->Token);
						return nullptr;
					}

					int32 DataX = X - 200;
					int32 DataY = Y + DataNodeOffsetY;
					UEdGraphPin* SourcePin = ResolveDataExpr(Graph, SourceExpr, DataX, DataY);
					if (SourcePin)
					{
						LocalVarToPinMap.Add(VarName, SourcePin);
						PersistentFrameVarMap.Add(VarName, SourcePin);
					}

					if (SourcePin && SourcePin->GetOwningNode())
					{
						UEdGraphNode* SourceNode = SourcePin->GetOwningNode();
						if (FindExecPin(SourceNode, EGPD_Input))
						{
							return SourceNode;
						}
					}
					return nullptr;
				}

				if (SourceExpr->IsFunctionCall())
				{
					UEdGraphNode* CallNode = CreateNodeForExpr(Graph, SourceExpr, X, Y);
					if (CallNode)
					{

						UEdGraphPin* ReturnPin = FindFirstDataOutputPin(CallNode);
						if (ReturnPin)
						{

							UClass* ReturnClass = Cast<UClass>(ReturnPin->PinType.PinSubCategoryObject.Get());
							UClass* VarClass = nullptr;
							if (DestExpr->PropertyRef)
							{
								if (FObjectPropertyBase* ObjProp = CastField<FObjectPropertyBase>(DestExpr->PropertyRef))
								{
									VarClass = ObjProp->PropertyClass;
								}
							}

							if (ReturnPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Object && !ReturnPin->PinType.IsContainer() && VarClass && ReturnClass &&
								VarClass != ReturnClass &&
								VarClass->IsChildOf(ReturnClass))
							{

								UK2Node_DynamicCast* AutoCast = NewObject<UK2Node_DynamicCast>(Graph);
								AutoCast->CreateNewGuid();
								AutoCast->TargetType = VarClass;
								AutoCast->SetPurity(true);
								AutoCast->NodePosX = X + 200;
								AutoCast->NodePosY = Y;
								Graph->AddNode(AutoCast, false, false);
								AutoCast->AllocateDefaultPins();

								UEdGraphPin* CastObjPin = AutoCast->GetCastSourcePin();
								if (CastObjPin)
								{
									ReturnPin->MakeLinkTo(CastObjPin);
									AutoCast->NotifyPinConnectionListChanged(CastObjPin);
								}

								UEdGraphPin* CastResultPin = AutoCast->GetCastResultPin();
								if (CastResultPin && !bIsMultiAssigned)
								{
									LocalVarToPinMap.Add(VarName, CastResultPin);
									PersistentFrameVarMap.Add(VarName, CastResultPin);
								}

							}
							else if (!bIsMultiAssigned)
							{
								LocalVarToPinMap.Add(VarName, ReturnPin);
								PersistentFrameVarMap.Add(VarName, ReturnPin);
							}
						}
					}
					return CallNode;
				}
				else if (SourceExpr->Token == EX_DynamicCast || SourceExpr->Token == EX_MetaCast ||
					SourceExpr->Token == EX_ObjToInterfaceCast ||
					SourceExpr->Token == EX_CrossInterfaceCast ||
					SourceExpr->Token == EX_InterfaceToObjCast)
				{

					int32 DataX = X - 200;
					int32 DataY = Y;
					UEdGraphPin* CastOutPin = ResolveDataExpr(Graph, SourceExpr, DataX, DataY);
					if (CastOutPin)
					{
						if (!bIsMultiAssigned)
						{
							LocalVarToPinMap.Add(VarName, CastOutPin);
							PersistentFrameVarMap.Add(VarName, CastOutPin);
						}

						UEdGraphNode* CastNode = CastOutPin->GetOwningNode();
						if (CastNode)
						{
							LastCastNode = CastNode;
							for (UEdGraphPin* Pin : CastNode->Pins)
							{
								if (Pin->Direction == EGPD_Output &&
									Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Boolean)
								{
									LocalVarToPinMap.Add(TEXT("K2Node_DynamicCast_bSuccess"), Pin);
									break;
								}
							}
						}
					}
					if (CastOutPin && CastOutPin->GetOwningNode())
					{
						return CastOutPin->GetOwningNode();
					}
					return nullptr;
				}
				else
				{

					bool bIsContextCall = false;
					if (SourceExpr->Token == EX_Context || SourceExpr->Token == EX_Context_FailSilent)
					{
						if (SourceExpr->Children.Num() > 0 && SourceExpr->Children[0] &&
							SourceExpr->Children[0]->IsFunctionCall())
						{
							bIsContextCall = true;
						}
					}

					if (bIsContextCall)
					{

						UEdGraphNode* CallNode = CreateNodeForExpr(Graph, SourceExpr, X, Y);
						if (CallNode && !bIsMultiAssigned)
						{
							UEdGraphPin* ReturnPin = FindFirstDataOutputPin(CallNode);
							if (ReturnPin)
							{
								LocalVarToPinMap.Add(VarName, ReturnPin);
								PersistentFrameVarMap.Add(VarName, ReturnPin);
							}
						}
						return CallNode;
					}

					if (SourceExpr->IsLiteral() && VarName.ToString().StartsWith(TEXT("Temp_")) &&
						!bIsMultiAssigned)
					{
						TempLiteralMap.Add(VarName, SourceExpr);
						return nullptr;
					}

					int32 DataX = X - 200;
					int32 DataY = Y;
					UEdGraphPin* SourcePin = ResolveDataExpr(Graph, SourceExpr, DataX, DataY);
					if (SourcePin && !bIsMultiAssigned)
					{
						LocalVarToPinMap.Add(VarName, SourcePin);
						PersistentFrameVarMap.Add(VarName, SourcePin);
					}

					if (SourcePin && SourcePin->GetOwningNode())
					{
						UEdGraphNode* SourceNode = SourcePin->GetOwningNode();
						if (FindExecPin(SourceNode, EGPD_Input))
						{
							return SourceNode;
						}
					}
					return nullptr;
				}
			}
		}

		if (DestExpr->Token == EX_StructMemberContext)
		{
			FProperty* MemberProp = DestExpr->PropertyRef;
			if (MemberProp && DestExpr->Children.Num() > 0 && DestExpr->Children[0])
			{
				TSharedPtr<FCASDecompiledExpr> StructVarExpr = DestExpr->Children[0];
				if (StructVarExpr->PropertyRef && (StructVarExpr->Token == EX_InstanceVariable || StructVarExpr->Token == EX_DefaultVariable))
                {
                    auto SetFields = NewObject<UK2Node_SetFieldsInStruct>(Graph);
                    SetFields->StructType = Cast<UScriptStruct>(MemberProp->GetOwnerStruct());
                    SetFields->CreateNewGuid();
                    SetFields->NodePosX = X; SetFields->NodePosY = Y;
                    Graph->AddNode(SetFields, false, false);
                    SetFields->AllocateDefaultPins();
                    for (auto& Optional : SetFields->ShowPinForProperties) Optional.bShowPin = Optional.PropertyName == MemberProp->GetFName();
                    SetFields->ReconstructNode();
                    int32 DataX = X - NodeSpacingX, DataY = Y;
                    ForceConnect(ResolveDataExpr(Graph, StructVarExpr, DataX, DataY), SetFields->FindPin(TEXT("StructRef")));
                    if (auto ValuePin = SetFields->FindPin(MemberProp->GetFName(), EGPD_Input))
                        if (!ApplyTempLiteralToPin(SourceExpr, ValuePin)) ForceConnect(ResolveDataExpr(Graph, SourceExpr, DataX, DataY), ValuePin);
                    return SetFields;
                }
				if (StructVarExpr->PropertyRef)
				{
					FName StructVarName = StructVarExpr->PropertyRef->GetFName();
					FName MemberName = MemberProp->GetFName();

					FStructMemberWrite& Entry = StructMemberAssignMap.FindOrAdd(StructVarName);
					if (!Entry.StructType)
					{
						Entry.StructType = Cast<UScriptStruct>(MemberProp->GetOwnerStruct());
					}
					Entry.MemberValues.Add(MemberName, SourceExpr);

					UE_LOG(LogGraphBuilder, Log, TEXT("MakeStruct accumulate: %s.%s (struct=%s)"),
						*StructVarName.ToString(), *MemberName.ToString(),
						Entry.StructType ? *Entry.StructType->GetName() : TEXT("unknown"));
					return nullptr; 
				}
			}
		}

		if (DestExpr->Token == EX_Context)
		{

			FProperty* TargetProp = nullptr;
			TSharedPtr<FCASDecompiledExpr> ContextObjectExpr;

			TSharedPtr<FCASDecompiledExpr> WalkExpr = DestExpr;
			while (WalkExpr)
			{
				if (WalkExpr->Token == EX_Context)
				{

					if (WalkExpr->ContextObject)
					{
						ContextObjectExpr = WalkExpr->ContextObject;
					}
					else if (WalkExpr->Children.Num() > 0)
					{
						ContextObjectExpr = WalkExpr->Children[0];
					}

					if (WalkExpr->Children.Num() > 0 && WalkExpr->Children.Last())
					{
						WalkExpr = WalkExpr->Children.Last();
						continue;
					}
					else if (WalkExpr->PropertyRef)
					{
						TargetProp = WalkExpr->PropertyRef;
						break;
					}
				}
				else if (WalkExpr->Token == EX_InstanceVariable && WalkExpr->PropertyRef)
				{
					TargetProp = WalkExpr->PropertyRef;
					break;
				}
				else if (WalkExpr->PropertyRef)
				{
					TargetProp = WalkExpr->PropertyRef;
					break;
				}
				break;
			}

			if (TargetProp)
			{
				FName VarName = TargetProp->GetFName();

				UK2Node_VariableSet* SetNode = NewObject<UK2Node_VariableSet>(Graph);
				SetNode->CreateNewGuid();

				UClass* OwnerClass = TargetProp->GetOwnerClass();
				if (OwnerClass)
				{
					SetNode->VariableReference.SetExternalMember(VarName, OwnerClass);
				}
				else
				{
					SetNode->VariableReference.SetSelfMember(VarName);
				}
				SetNode->NodePosX = X;
				SetNode->NodePosY = Y;
				SetNode->AllocateDefaultPins();
				Graph->AddNode(SetNode, false, false);

				if (ContextObjectExpr)
				{
					UEdGraphPin* TargetPin = FindDataPin(SetNode, EGPD_Input, UEdGraphSchema_K2::PN_Self);
					if (TargetPin)
					{
						int32 CtxX = X - 300;
						int32 CtxY = Y + DataNodeOffsetY;
						UEdGraphPin* CtxPin = ResolveDataExpr(Graph, ContextObjectExpr, CtxX, CtxY);
						if (CtxPin)
						{
							ForceConnect(CtxPin, TargetPin);
						}
					}
				}

				FName VarPinName = TargetProp->GetFName();
				UEdGraphPin* ValuePin = FindDataPin(SetNode, EGPD_Input, VarPinName);
				if (!ValuePin)
				{
					for (UEdGraphPin* Pin : SetNode->Pins)
					{
						if (Pin->Direction == EGPD_Input &&
							Pin->PinType.PinCategory != UEdGraphSchema_K2::PC_Exec &&
							Pin->PinName != UEdGraphSchema_K2::PN_Self)
						{
							ValuePin = Pin;
							break;
						}
					}
				}

				if (ValuePin)
				{
					int32 DataX = X - 200;
					int32 DataY = Y + DataNodeOffsetY;
					if (SourceExpr->IsLiteral())
					{
						if (SourceExpr->Token == EX_NoObject)
						{
							ValuePin->DefaultValue = TEXT("");
						}
						else if (SourceExpr->Token == EX_ObjectConst && SourceExpr->ObjectRef)
						{
							ValuePin->DefaultObject = SourceExpr->ObjectRef;
						}
						else
						{
							bool bEnumResolved = false;
							if ((SourceExpr->Token == EX_ByteConst || SourceExpr->Token == EX_IntConst ||
								SourceExpr->Token == EX_IntConstByte || SourceExpr->Token == EX_IntZero || SourceExpr->Token == EX_IntOne) &&
								ValuePin->PinType.PinCategory == UEdGraphSchema_K2::PC_Byte &&
								ValuePin->PinType.PinSubCategoryObject.IsValid())
							{
								UEnum* PinEnum = Cast<UEnum>(ValuePin->PinType.PinSubCategoryObject.Get());
								if (PinEnum)
								{
									int64 Val = (SourceExpr->Token == EX_ByteConst) ? SourceExpr->ByteValue : SourceExpr->IntValue;
									FString EnumName = PinEnum->GetNameStringByValue(Val);
									if (!UE427IsEmpty(EnumName))
									{
										ValuePin->DefaultValue = EnumName;
										bEnumResolved = true;
									}
								}
							}
							if (!bEnumResolved)
							{
								ValuePin->DefaultValue = SourceExpr->GetLiteralAsString();
							}
						}
					}
					else if (ApplyTempLiteralToPin(SourceExpr, ValuePin))
					{

					}
					else
					{
						UEdGraphPin* SourcePin = ResolveDataExpr(Graph, SourceExpr, DataX, DataY);
						ForceConnect(SourcePin, ValuePin);
					}
				}

				UE_LOG(LogGraphBuilder, Log, TEXT("Context-based property set: %s (via context)"), *VarName.ToString());
				X += NodeSpacingX;
				return SetNode;
			}
		}

		if (SourceExpr->IsFunctionCall())
		{
			return CreateNodeForExpr(Graph, SourceExpr, X, Y);
		}

		return nullptr;
	}

	case EX_LetValueOnPersistentFrame:
	{

		if (!Expr->PropertyRef || Expr->Children.Num() < 1) return nullptr;

		FName VarName = Expr->PropertyRef->GetFName();
		TSharedPtr<FCASDecompiledExpr> SourceExpr = Expr->Children[0];
		if (!SourceExpr) return nullptr;

		const bool bIsMultiAssignedPF = MultiAssignedTempVars.Contains(VarName);

		if (bIsMultiAssignedPF)
		{
			UK2Node_AssignmentStatement* AssignNode = NewObject<UK2Node_AssignmentStatement>(Graph);
			AssignNode->CreateNewGuid();
			AssignNode->NodePosX = X;
			AssignNode->NodePosY = Y;
			AssignNode->AllocateDefaultPins();
			Graph->AddNode(AssignNode, false, false);

			UEdGraphPin* VarPin = AssignNode->GetVariablePin();
			if (VarPin)
			{
				UEdGraphPin* TempVarOutPin = nullptr;
				if (UEdGraphPin** CachedPin = MultiAssignedTempPins.Find(VarName))
				{
					TempVarOutPin = *CachedPin;
				}
				else
				{
					UK2Node_TemporaryVariable* TempNode = NewObject<UK2Node_TemporaryVariable>(Graph);
					TempNode->CreateNewGuid();
					FEdGraphPinType VarType;
					GetDefault<UEdGraphSchema_K2>()->ConvertPropertyToPinType(Expr->PropertyRef, VarType);
					TempNode->VariableType = VarType;
					TempNode->NodePosX = X - NodeSpacingX / 2;
					TempNode->NodePosY = Y - 150;
					TempNode->AllocateDefaultPins();
					Graph->AddNode(TempNode, false, false);
					TempVarOutPin = FindFirstDataOutputPin(TempNode);
					if (TempVarOutPin)
					{
						MultiAssignedTempPins.Add(VarName, TempVarOutPin);
					}
				}
				if (TempVarOutPin)
				{
					VarPin->PinType = TempVarOutPin->PinType;
					if (!TryConnect(TempVarOutPin, VarPin))
						Warn(FString::Printf(TEXT("Cannot connect assignment variable %s in %s"), *VarName.ToString(), *Graph->GetName()));
					UEdGraphPin* ValPin = AssignNode->GetValuePin();
					if (ValPin)
					{
						ValPin->PinType = TempVarOutPin->PinType;
					}
				}
			}

			UEdGraphPin* ValPin = AssignNode->GetValuePin();
			UE_LOG(LogGraphBuilder, Log, TEXT("MultiAssign PersistFrame: Var=%s SourceToken=0x%02X IsLiteral=%d ValPin=%s PinCat=%s SubCatValid=%d"),
				*VarName.ToString(), (uint8)SourceExpr->Token, SourceExpr->IsLiteral() ? 1 : 0,
				ValPin ? TEXT("valid") : TEXT("NULL"),
				ValPin ? *ValPin->PinType.PinCategory.ToString() : TEXT("N/A"),
				(ValPin && ValPin->PinType.PinSubCategoryObject.IsValid()) ? 1 : 0);
			if (ValPin)
			{
				int32 DataX = X - 200;
				int32 DataY = Y + DataNodeOffsetY;
				if (SourceExpr->IsLiteral())
				{
					if (SourceExpr->Token == EX_NoObject)
					{
						ValPin->DefaultValue = TEXT("");
					}
					else if (SourceExpr->Token == EX_ObjectConst && SourceExpr->ObjectRef)
					{
						ValPin->DefaultObject = SourceExpr->ObjectRef;
					}
					else
					{

						bool bEnumResolved = false;
						if ((SourceExpr->Token == EX_ByteConst || SourceExpr->Token == EX_IntConst ||
							SourceExpr->Token == EX_IntConstByte || SourceExpr->Token == EX_IntZero || SourceExpr->Token == EX_IntOne) &&
							ValPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Byte &&
							ValPin->PinType.PinSubCategoryObject.IsValid())
						{
							UEnum* PinEnum = Cast<UEnum>(ValPin->PinType.PinSubCategoryObject.Get());
							if (PinEnum)
							{
								int64 Val = (SourceExpr->Token == EX_ByteConst) ? SourceExpr->ByteValue : SourceExpr->IntValue;
								FString EnumName = PinEnum->GetNameStringByValue(Val);
								UE_LOG(LogGraphBuilder, Log, TEXT("  PF EnumResolve: Enum=%s Val=%lld Name='%s'"),
									*PinEnum->GetName(), Val, *EnumName);
								if (!UE427IsEmpty(EnumName))
								{
									ValPin->DefaultValue = EnumName;
									bEnumResolved = true;
								}
							}
						}
						if (!bEnumResolved)
						{
							ValPin->DefaultValue = SourceExpr->GetLiteralAsString();
							UE_LOG(LogGraphBuilder, Log, TEXT("  PF Fallback literal: '%s'"), *ValPin->DefaultValue);
						}
					}
					UE_LOG(LogGraphBuilder, Log, TEXT("  PF After literal set: DefaultValue='%s'"), *ValPin->DefaultValue);

					DeferredPinDefaults.Add({AssignNode, ValPin->PinName, ValPin->DefaultValue, ValPin->DefaultObject});
				}
				else if (ApplyTempLiteralToPin(SourceExpr, ValPin))
				{

					UE_LOG(LogGraphBuilder, Log, TEXT("  PF ApplyTempLiteralToPin succeeded"));
					DeferredPinDefaults.Add({AssignNode, ValPin->PinName, ValPin->DefaultValue, ValPin->DefaultObject});
				}
				else
				{
					UEdGraphPin* SourcePin = ResolveDataExpr(Graph, SourceExpr, DataX, DataY);
					ForceConnect(SourcePin, ValPin);
					UE_LOG(LogGraphBuilder, Log, TEXT("  PF ResolveDataExpr path, SourcePin=%s"),
						SourcePin ? TEXT("valid") : TEXT("NULL"));
				}
			}

			X += NodeSpacingX;
			return AssignNode;
		}

		if (SourceExpr->IsFunctionCall() ||
			(SourceExpr->Token == EX_Context || SourceExpr->Token == EX_Context_FailSilent))
		{
			UEdGraphNode* CallNode = CreateNodeForExpr(Graph, SourceExpr, X, Y);
			if (CallNode && !bIsMultiAssignedPF)
			{
				UEdGraphPin* ReturnPin = FindFirstDataOutputPin(CallNode);
				if (ReturnPin)
				{
					LocalVarToPinMap.Add(VarName, ReturnPin);
					PersistentFrameVarMap.Add(VarName, ReturnPin);
				}
			}
			return CallNode;
		}
		else if (SourceExpr->Token == EX_DynamicCast || SourceExpr->Token == EX_MetaCast ||
			SourceExpr->Token == EX_ObjToInterfaceCast)
		{
			int32 DataX = X - 200;
			int32 DataY = Y;
			UEdGraphPin* CastOutPin = ResolveDataExpr(Graph, SourceExpr, DataX, DataY);
			if (CastOutPin)
			{
				if (!bIsMultiAssignedPF)
				{
					LocalVarToPinMap.Add(VarName, CastOutPin);
					PersistentFrameVarMap.Add(VarName, CastOutPin);
				}

				UEdGraphNode* CastNode = CastOutPin->GetOwningNode();
				if (CastNode)
				{
					LastCastNode = CastNode;
					for (UEdGraphPin* Pin : CastNode->Pins)
					{
						if (Pin->Direction == EGPD_Output &&
							Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Boolean)
						{

							LocalVarToPinMap.Add(TEXT("K2Node_DynamicCast_bSuccess"), Pin);
							break;
						}
					}
				}

				if (CastNode)
					return CastNode;
			}
			return nullptr;
		}
		else
		{

			if (SourceExpr->IsLiteral() && VarName.ToString().StartsWith(TEXT("Temp_")) &&
				!bIsMultiAssignedPF)
			{
				TempLiteralMap.Add(VarName, SourceExpr);
				return nullptr;
			}

			int32 DataX = X - 200;
			int32 DataY = Y;
			UEdGraphPin* SourcePin = ResolveDataExpr(Graph, SourceExpr, DataX, DataY);
			if (SourcePin && !bIsMultiAssignedPF)
			{
				LocalVarToPinMap.Add(VarName, SourcePin);
				PersistentFrameVarMap.Add(VarName, SourcePin);
			}
			return nullptr;
		}
	}

	case EX_SetMap:
	{
		if (UE427IsEmpty(Expr->Children) || !Expr->Children[0]) return nullptr;
		auto Target = Expr->Children[0];
		auto Property = CastField<FMapProperty>(Target->PropertyRef);
		if (!Property || Expr->Children.Num() % 2 != 1) { Warn(TEXT("Invalid map assignment")); return nullptr; }
		auto Map = NewObject<UK2Node_MakeMap>(Graph);
		Map->CreateNewGuid();
		Map->NumInputs = (Expr->Children.Num() - 1) / 2;
		Map->NodePosX = X - NodeSpacingX;
		Map->NodePosY = Y;
		Graph->AddNode(Map, false, false);
		Map->AllocateDefaultPins();
		const auto Schema = GetDefault<UEdGraphSchema_K2>();
		Schema->ConvertPropertyToPinType(Property, Map->GetOutputPin()->PinType);
		int32 Index = 1;
		for (auto Pin : Map->Pins)
		{
			if (Pin->Direction != EGPD_Input) continue;
			Schema->ConvertPropertyToPinType(Index % 2 ? Property->KeyProp : Property->ValueProp, Pin->PinType);
			Schema->SetPinAutogeneratedDefaultValueBasedOnType(Pin);
			auto Value = Expr->Children[Index++];
			int32 ValueX = X - NodeSpacingX * 2, ValueY = Y + Index * 60;
			if (!ApplyTempLiteralToPin(Value, Pin))
			{
				if (Value->IsLiteral() && Value->Token != EX_Self) Pin->DefaultValue = Value->GetLiteralAsString();
				else if (!TryConnect(ResolveDataExpr(Graph, Value, ValueX, ValueY), Pin)) Warn(TEXT("Cannot connect map entry"));
			}
		}
		if (Property->GetName().StartsWith(TEXT("K2Node_MakeMap_")) || Property->GetName().StartsWith(TEXT("K2Node_MakeVariable_")))
		{
			LocalVarToPinMap.Add(Property->GetFName(), Map->GetOutputPin());
			return nullptr;
		}
		int32 TargetX = X - NodeSpacingX, TargetY = Y - 150;
		auto TargetPin = ResolveDataExpr(Graph, Target, TargetX, TargetY);
		if (!TargetPin) { Warn(TEXT("Cannot resolve map assignment target")); return nullptr; }
		auto Assign = NewObject<UK2Node_AssignmentStatement>(Graph);
		Assign->CreateNewGuid();
		Assign->NodePosX = X;
		Assign->NodePosY = Y;
		Graph->AddNode(Assign, false, false);
		Assign->AllocateDefaultPins();
		Assign->GetVariablePin()->PinType = Map->GetOutputPin()->PinType;
		Assign->GetValuePin()->PinType = Map->GetOutputPin()->PinType;
		if (!TryConnect(TargetPin, Assign->GetVariablePin()) || !TryConnect(Map->GetOutputPin(), Assign->GetValuePin())) Warn(TEXT("Cannot wire map assignment"));
		X += NodeSpacingX;
		return Assign;
	}

	case EX_SetArray:
	{

		if (Expr->Children.Num() < 1) return nullptr;

		TSharedPtr<FCASDecompiledExpr> TargetExpr = Expr->Children[0];
		if (!TargetExpr || !TargetExpr->PropertyRef) return nullptr;

		FName ArrayVarName = TargetExpr->PropertyRef->GetFName();
		int32 NumElements = Expr->Children.Num() - 1;

		if (NumElements == 0 && ArrayVarName.ToString().StartsWith(TEXT("CallFunc_")))
		{
			return nullptr;
		}

		UK2Node_MakeArray* MakeArrayNode = NewObject<UK2Node_MakeArray>(Graph);
		MakeArrayNode->CreateNewGuid();
		MakeArrayNode->NumInputs = NumElements;
		MakeArrayNode->NodePosX = X;
		MakeArrayNode->NodePosY = Y;
		MakeArrayNode->AllocateDefaultPins();
		Graph->AddNode(MakeArrayNode, false, false);

		FArrayProperty* ArrayProp = CastField<FArrayProperty>(TargetExpr->PropertyRef);
		if (ArrayProp && ArrayProp->Inner)
		{
			FEdGraphPinType ElemPinType;
			GetDefault<UEdGraphSchema_K2>()->ConvertPropertyToPinType(ArrayProp->Inner, ElemPinType);

			for (UEdGraphPin* Pin : MakeArrayNode->Pins)
			{
				if (Pin->Direction == EGPD_Output)
				{
					Pin->PinType = ElemPinType;
					Pin->PinType.ContainerType = EPinContainerType::Array;
					break;
				}
			}

			for (UEdGraphPin* Pin : MakeArrayNode->Pins)
			{
				if (Pin->Direction == EGPD_Input &&
					Pin->PinType.PinCategory != UEdGraphSchema_K2::PC_Exec)
				{
					Pin->PinType = ElemPinType;
				}
			}
		}

		int32 ElemX = X - 250;
		int32 ElemY = Y;
		for (int32 i = 0; i < NumElements; i++)
		{
			TSharedPtr<FCASDecompiledExpr> ElemExpr = Expr->Children[i + 1];
			if (!ElemExpr) continue;

			FName ElemPinName = *FString::Printf(TEXT("[%d]"), i);
			UEdGraphPin* ElemPin = FindDataPin(MakeArrayNode, EGPD_Input, ElemPinName);
			if (!ElemPin)
			{

				ElemPinName = *FString::FromInt(i);
				ElemPin = FindDataPin(MakeArrayNode, EGPD_Input, ElemPinName);
			}
			if (!ElemPin)
			{

				int32 DataIdx = 0;
				for (UEdGraphPin* Pin : MakeArrayNode->Pins)
				{
					if (Pin->Direction == EGPD_Input &&
						Pin->PinType.PinCategory != UEdGraphSchema_K2::PC_Exec)
					{
						if (DataIdx == i) { ElemPin = Pin; break; }
						DataIdx++;
					}
				}
			}

			if (ElemPin)
			{

				if (ElemExpr->Token == EX_Self)
				{
					UK2Node_Self* SelfNode = NewObject<UK2Node_Self>(Graph);
					SelfNode->CreateNewGuid();
					SelfNode->NodePosX = ElemX;
					SelfNode->NodePosY = ElemY;
					SelfNode->AllocateDefaultPins();
					Graph->AddNode(SelfNode, false, false);
					UEdGraphPin* SelfOut = FindFirstDataOutputPin(SelfNode);
					ForceConnect(SelfOut, ElemPin);
					ElemY += NodeSpacingY / 3;
				}
				else if (ElemExpr->Token == EX_NoObject)
				{
					ElemPin->DefaultValue = TEXT("");
				}
				else if (ElemExpr->IsLiteral())
				{

					if ((ElemExpr->Token == EX_ByteConst || ElemExpr->Token == EX_IntConst ||
						ElemExpr->Token == EX_IntConstByte) &&
						ElemPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Byte &&
						ElemPin->PinType.PinSubCategoryObject.IsValid())
					{
						UEnum* PinEnum = Cast<UEnum>(ElemPin->PinType.PinSubCategoryObject.Get());
						if (PinEnum)
						{
							int64 Val = (ElemExpr->Token == EX_ByteConst) ? ElemExpr->ByteValue : ElemExpr->IntValue;
							FString EnumName = PinEnum->GetNameStringByValue(Val);
							ElemPin->DefaultValue = UE427IsEmpty(EnumName) ? ElemExpr->GetLiteralAsString() : EnumName;
						}
						else
						{
							ElemPin->DefaultValue = ElemExpr->GetLiteralAsString();
						}
					}
					else if (ElemExpr->Token == EX_ObjectConst)
					{
						if (ElemExpr->ObjectRef)
						{
							ElemPin->DefaultObject = ElemExpr->ObjectRef;
						}
					}
					else
					{
						ElemPin->DefaultValue = ElemExpr->GetLiteralAsString();
					}
				}
				else
				{
					UEdGraphPin* ElemOut = ResolveDataExpr(Graph, ElemExpr, ElemX, ElemY);
					ForceConnect(ElemOut, ElemPin);
					ElemY += NodeSpacingY / 3;
				}
			}
		}

		UEdGraphPin* ArrayOutPin = FindFirstDataOutputPin(MakeArrayNode);
		if (ArrayOutPin)
		{
			LocalVarToPinMap.Add(ArrayVarName, ArrayOutPin);
		}

		return nullptr;
	}

	case EX_Context:
	case EX_Context_FailSilent:
	{

		if (Expr->Children.Num() > 0 && Expr->Children[0])
		{
			TSharedPtr<FCASDecompiledExpr> InnerExpr = Expr->Children[0];

			if (InnerExpr->ContextObject && Expr->ContextObject
				&& InnerExpr->ContextObject.Get() != Expr->ContextObject.Get())
			{

				bool bWouldCycle = false;
				for (TSharedPtr<FCASDecompiledExpr> Walk = Expr->ContextObject; Walk; Walk = Walk->ContextObject)
				{
					if (Walk.Get() == InnerExpr->ContextObject.Get())
					{
						bWouldCycle = true;
						break;
					}
				}
				if (!bWouldCycle)
				{
					InnerExpr->ContextObject->ContextObject = Expr->ContextObject;
				}
			}
			else if (!InnerExpr->ContextObject)
			{
				InnerExpr->ContextObject = Expr->ContextObject;
			}

			if (InnerExpr->ContextObject && Expr->FieldRef)
			{
				if (!InnerExpr->ContextObject->FieldRef)
				{
					InnerExpr->ContextObject->FieldRef = Expr->FieldRef;
				}
			}

			return CreateNodeForExpr(Graph, InnerExpr, X, Y);
		}
		return nullptr;
	}

	case EX_MetaCast:
	case EX_DynamicCast:
	{
		if (!Expr->ClassRef) return nullptr;

		UK2Node_DynamicCast* CastNode = Expr->Token == EX_MetaCast ? NewObject<UK2Node_ClassDynamicCast>(Graph) : NewObject<UK2Node_DynamicCast>(Graph);
		CastNode->CreateNewGuid();
		CastNode->TargetType = Expr->ClassRef;
		CastNode->NodePosX = X;
		CastNode->NodePosY = Y;
		CastNode->AllocateDefaultPins();
		Graph->AddNode(CastNode, false, false);

		{
			UEdGraphPin* ObjPin = FindDataPin(CastNode, EGPD_Input, TEXT("Object"));
			if (!ObjPin)
			{
				for (UEdGraphPin* Pin : CastNode->Pins)
				{
					if (Pin->Direction == EGPD_Input && Pin->PinType.PinCategory != UEdGraphSchema_K2::PC_Exec)
					{
						ObjPin = Pin;
						break;
					}
				}
			}
			if (ObjPin)
			{
				ObjPin->PinType.PinCategory = Expr->Token == EX_MetaCast ? UEdGraphSchema_K2::PC_Class : UEdGraphSchema_K2::PC_Object;
				ObjPin->PinType.PinSubCategoryObject = UObject::StaticClass();
			}
		}

		if (Expr->Children.Num() > 0 && Expr->Children[0])
		{
			UEdGraphPin* ObjectPin = FindDataPin(CastNode, EGPD_Input, TEXT("Object"));
			if (!ObjectPin)
			{

				for (UEdGraphPin* Pin : CastNode->Pins)
				{
					if (Pin->Direction == EGPD_Input && Pin->PinType.PinCategory != UEdGraphSchema_K2::PC_Exec)
					{
						ObjectPin = Pin;
						break;
					}
				}
			}

			if (ObjectPin)
			{
				int32 DataX = X - 200;
				int32 DataY = Y + DataNodeOffsetY;
				UEdGraphPin* InputPin = ResolveDataExpr(Graph, Expr->Children[0], DataX, DataY);

				ForceConnect(InputPin, ObjectPin);
			}
		}

		return CastNode;
	}

	case EX_BindDelegate:
	{
		if (Expr->Children.Num() < 2) return nullptr;

		UK2Node_CreateDelegate* CreateDelegateNode = NewObject<UK2Node_CreateDelegate>(Graph);
		CreateDelegateNode->CreateNewGuid();
		CreateDelegateNode->SelectedFunctionName = Expr->NameValue;
		CreateDelegateNode->NodePosX = X - 200;
		CreateDelegateNode->NodePosY = Y + DataNodeOffsetY;
		CreateDelegateNode->AllocateDefaultPins();
		Graph->AddNode(CreateDelegateNode, false, false);

		if (Expr->Children[1] && Expr->Children[1]->Token != EX_Self)
		{
			UEdGraphPin* SelfPin = CreateDelegateNode->GetObjectInPin();
			if (SelfPin)
			{
				int32 DataX = X - 400;
				int32 DataY = Y + DataNodeOffsetY;
				UEdGraphPin* TargetPin = ResolveDataExpr(Graph, Expr->Children[1], DataX, DataY);
				ForceConnect(TargetPin, SelfPin);
			}
		}

		if (Expr->Children[0] && Expr->Children[0]->PropertyRef)
		{
			FName VarName = Expr->Children[0]->PropertyRef->GetFName();
			UEdGraphPin* OutputDelegatePin = CreateDelegateNode->GetDelegateOutPin();
			if (OutputDelegatePin)
			{
				LocalVarToPinMap.Add(VarName, OutputDelegatePin);
			}
		}

		return nullptr;
	}

	case EX_AddMulticastDelegate:
	case EX_RemoveMulticastDelegate:
	{
		if (Expr->Children.Num() < 2) return nullptr;

		TSharedPtr<FCASDecompiledExpr> DelegateExpr = Expr->Children[0];
		FProperty* DelegateProp = nullptr;
		bool bSelfContext = false;
		TSharedPtr<FCASDecompiledExpr> TargetObjectExpr;

		if (DelegateExpr)
		{
			if (DelegateExpr->Token == EX_InstanceVariable && DelegateExpr->PropertyRef)
			{

				DelegateProp = CastField<FProperty>(DelegateExpr->PropertyRef);
				bSelfContext = true;
			}
			else if (DelegateExpr->Token == EX_Context || DelegateExpr->Token == EX_Context_FailSilent)
			{

				TargetObjectExpr = DelegateExpr->ContextObject;

				if (DelegateExpr->FieldRef)
				{
					DelegateProp = CastField<FProperty>(DelegateExpr->FieldRef);
				}

				if (!DelegateProp && DelegateExpr->Children.Num() > 0 &&
					DelegateExpr->Children[0] && DelegateExpr->Children[0]->PropertyRef)
				{
					DelegateProp = CastField<FProperty>(DelegateExpr->Children[0]->PropertyRef);
				}
			}
		}

		if (!DelegateProp)
		{
			Warn(FString::Printf(TEXT("Could not extract delegate property for %s at offset 0x%04X"),
				Expr->Token == EX_AddMulticastDelegate ? TEXT("AddMulticastDelegate") : TEXT("RemoveMulticastDelegate"),
				Expr->StartOffset));
			return nullptr;
		}

		UClass* OwnerClass = DelegateProp->GetOwnerClass();

		UK2Node_BaseMCDelegate* DelegateNode = nullptr;
		if (Expr->Token == EX_AddMulticastDelegate)
		{
			DelegateNode = NewObject<UK2Node_AddDelegate>(Graph);
		}
		else
		{
			DelegateNode = NewObject<UK2Node_RemoveDelegate>(Graph);
		}

		DelegateNode->CreateNewGuid();
		DelegateNode->SetFromProperty(DelegateProp, bSelfContext, OwnerClass);
		DelegateNode->NodePosX = X;
		DelegateNode->NodePosY = Y;
		DelegateNode->AllocateDefaultPins();
		Graph->AddNode(DelegateNode, false, false);

		if (!bSelfContext && TargetObjectExpr)
		{
			UEdGraphPin* SelfPin = FindDataPin(DelegateNode, EGPD_Input, UEdGraphSchema_K2::PN_Self);
			if (SelfPin)
			{
				int32 DataX = X - 200;
				int32 DataY = Y + DataNodeOffsetY;
				UEdGraphPin* TargetPin = ResolveDataExpr(Graph, TargetObjectExpr, DataX, DataY);
				ForceConnect(TargetPin, SelfPin);
			}
		}

		UEdGraphPin* DelegatePin = DelegateNode->GetDelegatePin();
		if (DelegatePin && Expr->Children[1])
		{
			int32 DataX = X - 200;
			int32 DataY = Y + DataNodeOffsetY + NodeSpacingY / 2;
			UEdGraphPin* DelegateSourcePin = ResolveDataExpr(Graph, Expr->Children[1], DataX, DataY);
			ForceConnect(DelegateSourcePin, DelegatePin);
		}

		return DelegateNode;
	}

	case EX_ClearMulticastDelegate:
	{
		if (Expr->Children.Num() < 1) return nullptr;

		TSharedPtr<FCASDecompiledExpr> DelegateExpr = Expr->Children[0];
		FProperty* DelegateProp = nullptr;
		bool bSelfContext = false;
		TSharedPtr<FCASDecompiledExpr> TargetObjectExpr;

		if (DelegateExpr)
		{
			if (DelegateExpr->Token == EX_InstanceVariable && DelegateExpr->PropertyRef)
			{
				DelegateProp = CastField<FProperty>(DelegateExpr->PropertyRef);
				bSelfContext = true;
			}
			else if (DelegateExpr->Token == EX_Context || DelegateExpr->Token == EX_Context_FailSilent)
			{
				TargetObjectExpr = DelegateExpr->ContextObject;
				if (DelegateExpr->FieldRef)
				{
					DelegateProp = CastField<FProperty>(DelegateExpr->FieldRef);
				}
				if (!DelegateProp && DelegateExpr->Children.Num() > 0 &&
					DelegateExpr->Children[0] && DelegateExpr->Children[0]->PropertyRef)
				{
					DelegateProp = CastField<FProperty>(DelegateExpr->Children[0]->PropertyRef);
				}
			}
		}

		if (!DelegateProp)
		{
			Warn(FString::Printf(TEXT("Could not extract delegate property for ClearMulticastDelegate at offset 0x%04X"),
				Expr->StartOffset));
			return nullptr;
		}

		UClass* OwnerClass = DelegateProp->GetOwnerClass();

		UK2Node_ClearDelegate* ClearNode = NewObject<UK2Node_ClearDelegate>(Graph);
		ClearNode->CreateNewGuid();
		ClearNode->SetFromProperty(DelegateProp, bSelfContext, OwnerClass);
		ClearNode->NodePosX = X;
		ClearNode->NodePosY = Y;
		ClearNode->AllocateDefaultPins();
		Graph->AddNode(ClearNode, false, false);

		if (!bSelfContext && TargetObjectExpr)
		{
			UEdGraphPin* SelfPin = FindDataPin(ClearNode, EGPD_Input, UEdGraphSchema_K2::PN_Self);
			if (SelfPin)
			{
				int32 DataX = X - 200;
				int32 DataY = Y + DataNodeOffsetY;
				UEdGraphPin* TargetPin = ResolveDataExpr(Graph, TargetObjectExpr, DataX, DataY);
				ForceConnect(TargetPin, SelfPin);
			}
		}

		return ClearNode;
	}

	case EX_SwitchValue:
	{

		int32 DataX = X;
		int32 DataY = Y;
		ResolveDataExpr(Graph, Expr, DataX, DataY);
		return nullptr; 
	}

	default:

		UE_LOG(LogGraphBuilder, Verbose, TEXT("Skipping expression token 0x%02X at offset 0x%04X"),
			(uint8)Expr->Token, Expr->StartOffset);
		return nullptr;
	}
}

UEdGraphPin* FCASGraphBuilder::ResolveDataExpr(
	UEdGraph* Graph,
	TSharedPtr<FCASDecompiledExpr> Expr,
	int32& X, int32& Y)
{

	static const int32 MaxTailIterations = 512;
	int32 TailIterCount = 0;
	for (;;)
	{
	if (!Expr || !Graph) return nullptr;
	if (++TailIterCount > MaxTailIterations)
	{
		Warn(TEXT("ResolveDataExpr: exceeded maximum iteration depth (possible cycle in expression tree)"));
		return nullptr;
	}

	switch (Expr->Token)
	{
	case EX_MapConst:
	{
		if (!UE427IsEmpty(Expr->Children)) {
			Warn(TEXT("Nonempty map constant requires key/value reconstruction"));
			return nullptr;
		}
		auto Map = NewObject<UK2Node_MakeMap>(Graph);
		Map->CreateNewGuid();
		Map->NumInputs = 0;
		Map->NodePosX = X;
		Map->NodePosY = Y;
		Graph->AddNode(Map, false, false);
		Map->AllocateDefaultPins();
		X -= NodeSpacingX;
		return Map->GetOutputPin();
	}

	case EX_InstanceVariable:
	{
		if (!Expr->PropertyRef) return nullptr;

		struct FVarChainEntry
		{
			UK2Node_VariableGet* Node;
			UEdGraphPin* OutputPin;
			UEdGraphPin* SelfPin; 
		};
		TArray<FVarChainEntry, TInlineAllocator<32>> Chain;
		TSharedPtr<FCASDecompiledExpr> CurrentExpr = Expr;
		TSharedPtr<FCASDecompiledExpr> RemainingContext; 
		int32 NodeX = X;

		TSet<FCASDecompiledExpr*> Visited;

		while (CurrentExpr)
		{

			if (Visited.Contains(CurrentExpr.Get()))
			{
				Warn(TEXT("ResolveDataExpr: cycle detected in ContextObject chain, breaking"));
				break;
			}
			Visited.Add(CurrentExpr.Get());

			if (CurrentExpr->Token != EX_InstanceVariable || !CurrentExpr->PropertyRef)
			{

				RemainingContext = CurrentExpr;
				break;
			}

			FName VarName = CurrentExpr->PropertyRef->GetFName();
			FString VarStr = VarName.ToString();

			if (Chain.Num() == 0 && Cast<UFunction>(CurrentExpr->PropertyRef->GetOwnerStruct()) && MultiAssignedTempVars.Contains(VarName))
			{
				if (UEdGraphPin** CachedPin = MultiAssignedTempPins.Find(VarName))
				{
					if (*CachedPin) return *CachedPin;
				}
				UK2Node_TemporaryVariable* TempNode = NewObject<UK2Node_TemporaryVariable>(Graph);
				TempNode->CreateNewGuid();
				FEdGraphPinType VarType;
				GetDefault<UEdGraphSchema_K2>()->ConvertPropertyToPinType(CurrentExpr->PropertyRef, VarType);
				TempNode->VariableType = VarType;
				TempNode->NodePosX = NodeX;
				TempNode->NodePosY = Y;
				TempNode->AllocateDefaultPins();
				Graph->AddNode(TempNode, false, false);
				UEdGraphPin* OutPin = FindFirstDataOutputPin(TempNode);
				if (OutPin) MultiAssignedTempPins.Add(VarName, OutPin);
				return OutPin;
			}

			bool bIsCompilerGenerated = !VarStr.StartsWith(TEXT("K2Node_PropertyAccess")) &&
				(VarStr.Contains(TEXT("K2Node_")) || VarStr.Contains(TEXT("CallFunc_")) || VarStr.StartsWith(TEXT("Temp_")));

			if (bIsCompilerGenerated)
			{
				UEdGraphPin** FoundPin = LocalVarToPinMap.Find(VarName);
				if (!FoundPin) FoundPin = PersistentFrameVarMap.Find(VarName);
				if (FoundPin && *FoundPin)
				{

					if (Chain.Num() > 0 && Chain.Last().SelfPin)
					{
						ForceConnect(*FoundPin, Chain.Last().SelfPin);
					}
					else
					{
						return *FoundPin;
					}
					break;
				}
			}

			if (bIsCompilerGenerated)
			{

				auto EndsWithAtBoundary = [](const FString& Str, const FString& Key) -> bool
				{
					if (!Str.EndsWith(Key)) return false;
					int32 MatchStart = Str.Len() - Key.Len();
					return MatchStart == 0 || Str[MatchStart - 1] == TEXT('_');
				};

				UEdGraphPin* MatchedPin = nullptr;
				int32 BestKeyLen = 0;
				for (auto& KV : LocalVarToPinMap)
				{
					if (KV.Value)
					{
						FString KeyStr = KV.Key.ToString();
						if (KeyStr.Len() > BestKeyLen && EndsWithAtBoundary(VarStr, KeyStr))
						{
							MatchedPin = KV.Value;
							BestKeyLen = KeyStr.Len();
						}
					}
				}

				if (!MatchedPin)
				{
					int32 LastUnderscore = VarStr.Find(TEXT("_"), ESearchCase::CaseSensitive, ESearchDir::FromEnd);
					if (LastUnderscore != INDEX_NONE)
					{
						FString Suffix = VarStr.Mid(LastUnderscore + 1);
						if (Suffix.Len() > 0 && Suffix.Len() <= 3 && Suffix.IsNumeric())
						{
							FString StrippedVarStr = VarStr.Left(LastUnderscore);
							BestKeyLen = 0;
							for (auto& KV : LocalVarToPinMap)
							{
								if (KV.Value)
								{
									FString KeyStr = KV.Key.ToString();
									if (KeyStr.Len() > BestKeyLen && EndsWithAtBoundary(StrippedVarStr, KeyStr))
									{
										MatchedPin = KV.Value;
										BestKeyLen = KeyStr.Len();
									}
								}
							}
							if (MatchedPin)
							{

								LocalVarToPinMap.Add(VarName, MatchedPin);
							}
						}
					}
				}
				if (MatchedPin)
				{
					if (Chain.Num() > 0 && Chain.Last().SelfPin)
					{
						ForceConnect(MatchedPin, Chain.Last().SelfPin);
					}
					else
					{
						return MatchedPin;
					}
					break;
				}

				if (Chain.Num() == 0) return nullptr;
				break;
			}

			UK2Node_VariableGet* GetNode = NewObject<UK2Node_VariableGet>(Graph);
			GetNode->CreateNewGuid();

			if (CurrentExpr->ContextObject)
			{
				UClass* OwnerClass = CurrentExpr->PropertyRef->GetOwnerClass();
				if (OwnerClass)
				{
					GetNode->VariableReference.SetExternalMember(VarName, OwnerClass);
				}
				else
				{
					GetNode->VariableReference.SetSelfMember(VarName);
				}
			}
			else
			{
				GetNode->VariableReference.SetSelfMember(VarName);
			}

			GetNode->NodePosX = NodeX;
			GetNode->NodePosY = Y;
			GetNode->AllocateDefaultPins();

			UEdGraphPin* VarOutPin = FindFirstDataOutputPin(GetNode);
			if (VarOutPin && CurrentExpr->PropertyRef)
			{
				FEdGraphPinType CorrectType;
				GetDefault<UEdGraphSchema_K2>()->ConvertPropertyToPinType(CurrentExpr->PropertyRef, CorrectType);
				if (VarOutPin->PinType.PinCategory != CorrectType.PinCategory ||
					VarOutPin->PinType.PinSubCategoryObject != CorrectType.PinSubCategoryObject)
				{
					VarOutPin->PinType = CorrectType;
				}
			}
			else if (!VarOutPin && CurrentExpr->PropertyRef)
			{
				FEdGraphPinType CorrectType;
				GetDefault<UEdGraphSchema_K2>()->ConvertPropertyToPinType(CurrentExpr->PropertyRef, CorrectType);
				VarOutPin = GetNode->CreatePin(EGPD_Output, CorrectType, VarName);
			}

			Graph->AddNode(GetNode, false, false);

			FVarChainEntry Entry;
			Entry.Node = GetNode;
			Entry.OutputPin = FindFirstDataOutputPin(GetNode);
			Entry.SelfPin = CurrentExpr->ContextObject
				? FindDataPin(GetNode, EGPD_Input, UEdGraphSchema_K2::PN_Self)
				: nullptr;
			Chain.Add(Entry);

			NodeX -= 200;

			if (!CurrentExpr->ContextObject)
				break;

			CurrentExpr = CurrentExpr->ContextObject;
		}

		for (int32 i = Chain.Num() - 1; i > 0; --i)
		{
			if (Chain[i].OutputPin && Chain[i - 1].SelfPin)
			{
				ForceConnect(Chain[i].OutputPin, Chain[i - 1].SelfPin);
			}
		}

		if (RemainingContext && Chain.Num() > 0 && Chain.Last().SelfPin &&
			!(RemainingContext->Token == EX_ObjectConst && ApplyTempLiteralToPin(RemainingContext, Chain.Last().SelfPin)))
		{
			int32 CtxX = NodeX;
			int32 CtxY = Y;
			UEdGraphPin* BottomOut = ResolveDataExpr(Graph, RemainingContext, CtxX, CtxY);
			ForceConnect(BottomOut, Chain.Last().SelfPin);
		}

		X -= NodeSpacingX / 2;
		return Chain.Num() == 0 ? nullptr : Chain[0].OutputPin;
	}

	case EX_LocalVariable:
	case EX_DefaultVariable:
	case EX_LocalOutVariable:
	{
		if (!Expr->PropertyRef) return nullptr;

		FName VarName = Expr->PropertyRef->GetFName();

		if (MultiAssignedTempVars.Contains(VarName))
		{
			if (UEdGraphPin** CachedPin = MultiAssignedTempPins.Find(VarName))
			{
				if (*CachedPin) return *CachedPin;
			}

			UK2Node_TemporaryVariable* TempNode = NewObject<UK2Node_TemporaryVariable>(Graph);
			TempNode->CreateNewGuid();
			FEdGraphPinType VarType;
			GetDefault<UEdGraphSchema_K2>()->ConvertPropertyToPinType(Expr->PropertyRef, VarType);
			TempNode->VariableType = VarType;
			TempNode->NodePosX = X;
			TempNode->NodePosY = Y;
			TempNode->AllocateDefaultPins();
			Graph->AddNode(TempNode, false, false);
			UEdGraphPin* OutPin = FindFirstDataOutputPin(TempNode);
			if (OutPin) MultiAssignedTempPins.Add(VarName, OutPin);
			X -= NodeSpacingX / 2;
			return OutPin;
		}

		if (UEdGraphPin** FoundPin = LocalVarToPinMap.Find(VarName))
		{
			if (*FoundPin)
			{
				return *FoundPin;
			}
		}

		if (UEdGraphPin** PersistPin = PersistentFrameVarMap.Find(VarName))
		{
			if (*PersistPin)
			{
				return *PersistPin;
			}
		}

		if (FStructMemberWrite* StructWrite = StructMemberAssignMap.Find(VarName))
		{
			if (StructWrite->StructType)
			{

				if (UEdGraphPin** CachedPin = LocalVarToPinMap.Find(VarName))
				{
					if (*CachedPin) return *CachedPin;
				}

				UK2Node_MakeStruct* MakeNode = NewObject<UK2Node_MakeStruct>(Graph);
				MakeNode->CreateNewGuid();
				MakeNode->StructType = StructWrite->StructType;
				MakeNode->bMadeAfterOverridePinRemoval = true;
				MakeNode->NodePosX = X;
				MakeNode->NodePosY = Y;
				MakeNode->AllocateDefaultPins();
				Graph->AddNode(MakeNode, false, false);

				for (auto& MemberPair : StructWrite->MemberValues)
				{
					UEdGraphPin* MemberPin = MakeNode->FindPin(MemberPair.Key, EGPD_Input);
					if (!MemberPin)
					{

						for (UEdGraphPin* Pin : MakeNode->Pins)
						{
							if (Pin->Direction == EGPD_Input &&
								Pin->PinType.PinCategory != UEdGraphSchema_K2::PC_Exec &&
								Pin->GetFName() == MemberPair.Key)
							{
								MemberPin = Pin;
								break;
							}
						}
					}

					if (MemberPin && MemberPair.Value)
					{
						TSharedPtr<FCASDecompiledExpr> ValExpr = MemberPair.Value;
						if (ValExpr->IsLiteral())
						{
							if (ValExpr->Token == EX_True)
								MemberPin->DefaultValue = TEXT("true");
							else if (ValExpr->Token == EX_False)
								MemberPin->DefaultValue = TEXT("false");
							else if (ValExpr->Token == EX_NoObject)
								MemberPin->DefaultValue = TEXT("");
							else if (ValExpr->Token == EX_ObjectConst && ValExpr->ObjectRef)
								MemberPin->DefaultObject = ValExpr->ObjectRef;
							else
							{

								bool bEnumResolved = false;
								if ((ValExpr->Token == EX_ByteConst || ValExpr->Token == EX_IntConst) &&
									MemberPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Byte &&
									MemberPin->PinType.PinSubCategoryObject.IsValid())
								{
									UEnum* PinEnum = Cast<UEnum>(MemberPin->PinType.PinSubCategoryObject.Get());
									if (PinEnum)
									{
										int64 Val = (ValExpr->Token == EX_ByteConst) ? ValExpr->ByteValue : ValExpr->IntValue;
										FString EnumName = PinEnum->GetNameStringByValue(Val);
										if (!UE427IsEmpty(EnumName))
										{
											MemberPin->DefaultValue = EnumName;
											bEnumResolved = true;
										}
									}
								}
								if (!bEnumResolved)
								{
									MemberPin->DefaultValue = ValExpr->GetLiteralAsString();
								}
							}
						}
						else
						{

							int32 DataX = X - 200;
							int32 DataY = Y + DataNodeOffsetY;
							UEdGraphPin* SourcePin = ResolveDataExpr(Graph, ValExpr, DataX, DataY);
							if (SourcePin)
							{
								ForceConnect(SourcePin, MemberPin);
							}
						}
					}
				}

				UEdGraphPin* OutputPin = MakeNode->FindPin(StructWrite->StructType->GetFName(), EGPD_Output);
				if (!OutputPin)
				{
					OutputPin = FindFirstDataOutputPin(MakeNode);
				}

				if (OutputPin)
				{
					LocalVarToPinMap.Add(VarName, OutputPin);
				}

				UE_LOG(LogGraphBuilder, Log, TEXT("Created MakeStruct node for '%s' (type=%s, %d members)"),
					*VarName.ToString(), *StructWrite->StructType->GetName(),
					StructWrite->MemberValues.Num());

				X -= NodeSpacingX;
				return OutputPin;
			}
		}

		FString VarStr = VarName.ToString();
		if (VarStr.Contains(TEXT("K2Node_")) || VarStr.Contains(TEXT("CallFunc_")) ||
			VarStr.StartsWith(TEXT("Temp_")))
		{

			auto EndsWithAtBoundary = [](const FString& Str, const FString& Key) -> bool
			{
				if (!Str.EndsWith(Key)) return false;
				int32 MatchStart = Str.Len() - Key.Len();
				return MatchStart == 0 || Str[MatchStart - 1] == TEXT('_');
			};

			UEdGraphPin* BestMatch = nullptr;
			int32 BestKeyLen = 0;
			for (auto& KV : LocalVarToPinMap)
			{
				if (KV.Value)
				{
					FString KeyStr = KV.Key.ToString();
					if (KeyStr.Len() > BestKeyLen && EndsWithAtBoundary(VarStr, KeyStr))
					{
						BestMatch = KV.Value;
						BestKeyLen = KeyStr.Len();
					}
				}
			}
			if (BestMatch)
			{
				return BestMatch;
			}

			int32 LastUnderscore = VarStr.Find(TEXT("_"), ESearchCase::CaseSensitive, ESearchDir::FromEnd);
			if (LastUnderscore != INDEX_NONE)
			{
				FString Suffix = VarStr.Mid(LastUnderscore + 1);
				if (Suffix.Len() > 0 && Suffix.Len() <= 3 && Suffix.IsNumeric())
				{
					FString StrippedVarStr = VarStr.Left(LastUnderscore);
					BestMatch = nullptr;
					BestKeyLen = 0;
					for (auto& KV : LocalVarToPinMap)
					{
						if (KV.Value)
						{
							FString KeyStr = KV.Key.ToString();
							if (KeyStr.Len() > BestKeyLen && EndsWithAtBoundary(StrippedVarStr, KeyStr))
							{
								BestMatch = KV.Value;
								BestKeyLen = KeyStr.Len();
							}
						}
					}
					if (BestMatch)
					{

						LocalVarToPinMap.Add(VarName, BestMatch);
						return BestMatch;
					}
				}
			}
			return nullptr;
		}

		bool bIsInstanceVariable = false;
		if (Expr->Token == EX_DefaultVariable)
		{
			bIsInstanceVariable = true;
		}
		else if (Expr->Token == EX_LocalVariable || Expr->Token == EX_LocalOutVariable)
		{

			if (CurrentOriginalClass && Expr->PropertyRef && Cast<UClass>(Expr->PropertyRef->GetOwnerStruct()))
			{
				bIsInstanceVariable = (FindFProperty<FProperty>(CurrentOriginalClass, VarName) == Expr->PropertyRef);
			}
		}

		if (bIsInstanceVariable)
		{

			UK2Node_VariableGet* GetNode = NewObject<UK2Node_VariableGet>(Graph);
			GetNode->CreateNewGuid();
			GetNode->VariableReference.SetSelfMember(VarName);
			GetNode->NodePosX = X;
			GetNode->NodePosY = Y;
			GetNode->AllocateDefaultPins();

			UEdGraphPin* VarOutPin = FindFirstDataOutputPin(GetNode);
			if (VarOutPin && Expr->PropertyRef)
			{
				FEdGraphPinType CorrectType;
				GetDefault<UEdGraphSchema_K2>()->ConvertPropertyToPinType(Expr->PropertyRef, CorrectType);
				if (VarOutPin->PinType.PinCategory != CorrectType.PinCategory ||
					VarOutPin->PinType.PinSubCategoryObject != CorrectType.PinSubCategoryObject)
				{
					VarOutPin->PinType = CorrectType;
				}
			}
			else if (!VarOutPin && Expr->PropertyRef)
			{
				FEdGraphPinType CorrectType;
				GetDefault<UEdGraphSchema_K2>()->ConvertPropertyToPinType(Expr->PropertyRef, CorrectType);
				VarOutPin = GetNode->CreatePin(EGPD_Output, CorrectType, VarName);
			}

			Graph->AddNode(GetNode, false, false);
			X -= NodeSpacingX / 2;
			return VarOutPin ? VarOutPin : FindFirstDataOutputPin(GetNode);
		}
		else
		{

			const bool bIsOutputParameter = Expr->PropertyRef && Expr->PropertyRef->HasAnyPropertyFlags(CPF_OutParm | CPF_ReturnParm)
				&& !Expr->PropertyRef->HasAnyPropertyFlags(CPF_ReferenceParm);
			if (!Expr->PropertyRef || !Expr->PropertyRef->GetOwner<UFunction>() || (Expr->PropertyRef->HasAnyPropertyFlags(CPF_Parm) && !bIsOutputParameter))
				Warn(FString::Printf(TEXT("Local variable '%s' not found in LocalVarToPinMap — creating typed local node"), *VarName.ToString()));

			UK2Node_TemporaryVariable* TempNode = NewObject<UK2Node_TemporaryVariable>(Graph);
			TempNode->CreateNewGuid();

			FEdGraphPinType VarType;
			GetDefault<UEdGraphSchema_K2>()->ConvertPropertyToPinType(Expr->PropertyRef, VarType);
			TempNode->VariableType = VarType;

			TempNode->NodePosX = X;
			TempNode->NodePosY = Y;
			TempNode->AllocateDefaultPins();
			Graph->AddNode(TempNode, false, false);

			UEdGraphPin* OutPin = FindFirstDataOutputPin(TempNode);
			if (OutPin)
			{

				LocalVarToPinMap.Add(VarName, OutPin);
			}

			X -= NodeSpacingX / 2;
			return OutPin;
		}
	}

	case EX_Self:
	{
		UK2Node_Self* SelfNode = NewObject<UK2Node_Self>(Graph);
		SelfNode->CreateNewGuid();
		SelfNode->NodePosX = X;
		SelfNode->NodePosY = Y;
		SelfNode->AllocateDefaultPins();
		Graph->AddNode(SelfNode, false, false);

		X -= NodeSpacingX / 2;
		return FindFirstDataOutputPin(SelfNode);
	}

	case EX_FinalFunction:
	case EX_LocalFinalFunction:
	case EX_CallMath:
	{
		if (!Expr->FunctionRef) return nullptr;

		FString FuncName = Expr->FunctionRef->GetName();
		bool bIsSubsystemGetter = FuncName.Contains(TEXT("GetGameInstanceSubsystem")) ||
			FuncName.Contains(TEXT("GetWorldSubsystem")) ||
			FuncName.Contains(TEXT("GetLocalPlayerSubsystem")) ||
			FuncName.Contains(TEXT("GetEngineSubsystem"));

		UClass* SubsystemClass = nullptr;
		if (bIsSubsystemGetter)
		{

			for (const TSharedPtr<FCASDecompiledExpr>& Arg : Expr->Children)
			{
				if (Arg && Arg->Token == EX_ObjectConst && Arg->ObjectRef)
				{
					SubsystemClass = Cast<UClass>(Arg->ObjectRef);
					if (SubsystemClass) break;
				}
			}
		}

		bool bIsArrayLibFunc = false;
		{
			UClass* FuncClass = Expr->FunctionRef->GetOwnerClass();
			if (FuncClass && FuncClass->GetName().Contains(TEXT("KismetArrayLibrary")) && !UE427IsEmpty(Expr->FunctionRef->GetMetaData(TEXT("ArrayParm"))))
			{
				bIsArrayLibFunc = true;
			}
		}
		UK2Node_CallFunction* CallNode = bIsArrayLibFunc
			? NewObject<UK2Node_CallArrayFunction>(Graph)
			: NewObject<UK2Node_CallFunction>(Graph);
		CallNode->CreateNewGuid();

		UClass* FuncOwner = Expr->FunctionRef->GetOwnerClass();
		bool bIsBPGCFunc = FuncOwner && Cast<UBlueprintGeneratedClass>(FuncOwner) != nullptr;
		bool bIsSelfBPGCFunc = bIsBPGCFunc && CurrentOriginalClass && CurrentOriginalClass->IsChildOf(FuncOwner);

		if (!bIsSelfBPGCFunc && bIsBPGCFunc && CurrentOriginalClass && !Expr->ContextObject)
		{
			if (CurrentOriginalClass->ImplementsInterface(FuncOwner))
			{
				bIsSelfBPGCFunc = true;
			}
			else
			{
				for (const FImplementedInterface& Iface : CurrentOriginalClass->Interfaces)
				{
					if (Iface.Class == FuncOwner ||
						(Iface.Class && Iface.Class->GetFName() == FuncOwner->GetFName()))
					{
						bIsSelfBPGCFunc = true;
						break;
					}
				}
			}
		}

		CallNode->bIsPureFunc = Expr->FunctionRef->HasAnyFunctionFlags(FUNC_BlueprintPure);
		CallNode->bIsConstFunc = Expr->FunctionRef->HasAnyFunctionFlags(FUNC_Const);

		if (bIsSelfBPGCFunc)
		{

			CallNode->FunctionReference.SetExternalMember(Expr->FunctionRef->GetFName(), FuncOwner);
		}
		else
		{
			UClass* RefClass = FuncOwner ? FuncOwner : CurrentOriginalClass;
			CallNode->FunctionReference.SetExternalMember(Expr->FunctionRef->GetFName(), RefClass);
		}

		CallNode->NodePosX = X;
		CallNode->NodePosY = Y;
		CallNode->AllocateDefaultPins();
		FixStructPinDefaults(CallNode);
		Graph->AddNode(CallNode, false, false);

		if (bIsSelfBPGCFunc)
		{

			CallNode->FunctionReference.SetSelfMember(Expr->FunctionRef->GetFName());

			UEdGraphPin* SelfPin = FindDataPin(CallNode, EGPD_Input, UEdGraphSchema_K2::PN_Self);
			if (SelfPin)
			{
				CallNode->Pins.Remove(SelfPin);
				SelfPin->MarkPendingKill();
			}
		}

		int32 ArgX = X - 200;
		int32 ArgY = Y;
		int32 ArgIndex = 0;

		for (const TSharedPtr<FCASDecompiledExpr>& Arg : Expr->Children)
		{
			if (!Arg) { ArgIndex++; continue; }

			FName PinName = GetPinNameForFunctionParam(Expr->FunctionRef, ArgIndex);
			UEdGraphPin* ArgPin = nullptr;

			if (PinName != NAME_None)
			{
				ArgPin = FindDataPin(CallNode, EGPD_Input, PinName);
			}

			if (!ArgPin)
			{
				int32 DataPinIdx = 0;
				for (UEdGraphPin* Pin : CallNode->Pins)
				{
					if (Pin->Direction == EGPD_Input &&
						Pin->PinType.PinCategory != UEdGraphSchema_K2::PC_Exec &&
						Pin->PinName != UEdGraphSchema_K2::PN_Self)
					{
						if (DataPinIdx == ArgIndex)
						{
							ArgPin = Pin;
							break;
						}
						DataPinIdx++;
					}
				}
			}

			if (ArgPin)
			{
				if ((Arg->Token == EX_StructConst || Arg->Token == EX_VectorConst) && ArgPin->PinType.bIsReference)
				{
					ForceConnect(ResolveDataExpr(Graph, Arg, ArgX, ArgY), ArgPin);
				}
				else if (Arg->IsLiteral())
				{

					bool bEnumResolved = false;
					if ((Arg->Token == EX_ByteConst || Arg->Token == EX_IntConst ||
						Arg->Token == EX_IntConstByte || Arg->Token == EX_IntZero || Arg->Token == EX_IntOne) &&
						ArgPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Byte &&
						ArgPin->PinType.PinSubCategoryObject.IsValid())
					{
						UEnum* PinEnum = Cast<UEnum>(ArgPin->PinType.PinSubCategoryObject.Get());
						if (PinEnum)
						{
							int64 Val = (Arg->Token == EX_ByteConst) ? Arg->ByteValue : Arg->IntValue;
							FString EnumName = PinEnum->GetNameStringByValue(Val);
							if (!UE427IsEmpty(EnumName))
							{
								ArgPin->DefaultValue = EnumName;
								bEnumResolved = true;
							}
						}
					}
					if (!bEnumResolved)
					{
						ArgPin->DefaultValue = Arg->GetLiteralAsString();
					}
					if (Arg->Token == EX_ObjectConst && Arg->ObjectRef)
					{
						ArgPin->DefaultObject = Arg->ObjectRef;
						ArgPin->GetOwningNode()->PinDefaultValueChanged(ArgPin);
					}
				}
				else if (ApplyTempLiteralToPin(Arg, ArgPin))
				{

				}
				else
				{
					UEdGraphPin* ArgOut = ResolveDataExpr(Graph, Arg, ArgX, ArgY);
					if (ArgOut)
					{
						TryConnect(ArgOut, ArgPin);
					}
					else if (Arg->PropertyRef &&
						(Arg->Token == EX_LocalVariable || Arg->Token == EX_LocalOutVariable))
					{

						DeferredDataWires.Add(TPair<UEdGraphPin*, FName>(
							ArgPin, Arg->PropertyRef->GetFName()));
					}
					ArgY += NodeSpacingY / 3;
				}
			}
			ArgIndex++;
		}

		if (Expr->ContextObject && Expr->ContextObject->Token != EX_ObjectConst)
		{
			UEdGraphPin* TargetPin = FindDataPin(CallNode, EGPD_Input, UEdGraphSchema_K2::PN_Self);

			if (!TargetPin)
			{
				TargetPin = FindDataPin(CallNode, EGPD_Input, TEXT("TargetArray"));
			}
			if (!TargetPin)
			{
				for (UEdGraphPin* Pin : CallNode->Pins)
				{
					if (Pin->Direction == EGPD_Input &&
						Pin->PinType.PinCategory != UEdGraphSchema_K2::PC_Exec &&
						Pin->LinkedTo.Num() == 0)
					{
						TargetPin = Pin;
						break;
					}
				}
			}

			if (TargetPin)
			{
				UEdGraphPin* CtxOut = ResolveDataExpr(Graph, Expr->ContextObject, ArgX, ArgY);
				ForceConnect(CtxOut, TargetPin);
			}
		}

		X -= NodeSpacingX / 2;

		{
			UClass* FuncClass = Expr->FunctionRef->GetOwnerClass();
			if (FuncClass && FuncClass->GetName().Contains(TEXT("KismetArrayLibrary")) && !UE427IsEmpty(Expr->FunctionRef->GetMetaData(TEXT("ArrayParm"))))
			{

				FEdGraphPinType ElemType;
				bool bFoundElemType = false;

				if (Expr->ContextObject && Expr->ContextObject->PropertyRef)
				{
					FArrayProperty* ArrayProp = CastField<FArrayProperty>(Expr->ContextObject->PropertyRef);
					if (ArrayProp && ArrayProp->Inner)
					{
						GetDefault<UEdGraphSchema_K2>()->ConvertPropertyToPinType(ArrayProp->Inner, ElemType);
						bFoundElemType = true;
					}
				}

				if (!bFoundElemType)
				{

					UEdGraphPin* ArrayPin = FindDataPin(CallNode, EGPD_Input, TEXT("TargetArray"));
					if (!ArrayPin || ArrayPin->LinkedTo.Num() == 0)
					{
						ArrayPin = FindDataPin(CallNode, EGPD_Input, TEXT("Target Array"));
					}
					if (!ArrayPin || ArrayPin->LinkedTo.Num() == 0)
					{

						for (UEdGraphPin* Pin : CallNode->Pins)
						{
							if (Pin->Direction == EGPD_Input &&
								Pin->PinType.PinCategory != UEdGraphSchema_K2::PC_Exec &&
								Pin->LinkedTo.Num() > 0 &&
								(Pin->PinType.ContainerType == EPinContainerType::Array ||
									(Pin->LinkedTo[0]->PinType.ContainerType == EPinContainerType::Array)))
							{
								ArrayPin = Pin;
								break;
							}
						}
					}
					if (ArrayPin && ArrayPin->LinkedTo.Num() > 0)
					{
						FEdGraphPinType ConnectedType = ArrayPin->LinkedTo[0]->PinType;
						if (ConnectedType.ContainerType == EPinContainerType::Array)
						{
							ElemType = ConnectedType;
							ElemType.ContainerType = EPinContainerType::None;
							bFoundElemType = true;
						}
					}
				}

				if (bFoundElemType)
				{
					for (UEdGraphPin* Pin : CallNode->Pins)
					{
						if (Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Wildcard)
						{
							if (Pin->PinType.ContainerType == EPinContainerType::Array)
							{

								Pin->PinType = ElemType;
								Pin->PinType.ContainerType = EPinContainerType::Array;
							}
							else
							{

								Pin->PinType = ElemType;
							}
						}
					}
				}
			}
		}

		if (bIsSubsystemGetter && SubsystemClass)
		{
			UEdGraphPin* BaseReturnPin = nullptr;
			for (UEdGraphPin* Pin : CallNode->Pins)
			{
				if (Pin->Direction == EGPD_Output &&
					Pin->PinName == UEdGraphSchema_K2::PN_ReturnValue)
				{
					BaseReturnPin = Pin;
					break;
				}
			}

			if (BaseReturnPin)
			{
				UClass* ReturnClass = Cast<UClass>(BaseReturnPin->PinType.PinSubCategoryObject.Get());
				if (ReturnClass && SubsystemClass != ReturnClass &&
					SubsystemClass->IsChildOf(ReturnClass))
				{

					UK2Node_DynamicCast* CastNode = NewObject<UK2Node_DynamicCast>(Graph);
					CastNode->CreateNewGuid();
					CastNode->TargetType = SubsystemClass;
					CastNode->SetPurity(true);
					CastNode->NodePosX = X + 200;
					CastNode->NodePosY = Y;
					Graph->AddNode(CastNode, false, false);
					CastNode->AllocateDefaultPins();

					UEdGraphPin* CastObjPin = CastNode->GetCastSourcePin();
					if (CastObjPin)
					{
						BaseReturnPin->MakeLinkTo(CastObjPin);
						CastNode->NotifyPinConnectionListChanged(CastObjPin);
					}

					UEdGraphPin* CastResultPin = CastNode->GetCastResultPin();
					if (CastResultPin)
					{
						return CastResultPin;
					}
				}

				return BaseReturnPin;
			}
		}

		UEdGraphPin* ReturnPin = nullptr;
		for (UEdGraphPin* Pin : CallNode->Pins)
		{
			if (Pin->Direction == EGPD_Output && Pin->PinType.PinCategory != UEdGraphSchema_K2::PC_Exec)
			{
				if (Pin->PinName == UEdGraphSchema_K2::PN_ReturnValue)
				{
					return Pin; 
				}
				if (!ReturnPin)
				{
					ReturnPin = Pin;
				}
			}
		}
		return ReturnPin;
	}

	case EX_MetaCast:
	case EX_DynamicCast:
	case EX_ObjToInterfaceCast:
	case EX_CrossInterfaceCast:
	case EX_InterfaceToObjCast:
	{
		if (!Expr->ClassRef) return nullptr;

		UK2Node_DynamicCast* CastNode = Expr->Token == EX_MetaCast ? NewObject<UK2Node_ClassDynamicCast>(Graph) : NewObject<UK2Node_DynamicCast>(Graph);
		CastNode->CreateNewGuid();
		CastNode->TargetType = Expr->ClassRef;
		if (Expr->Token == EX_ObjToInterfaceCast || Expr->Token == EX_CrossInterfaceCast || Expr->Token == EX_InterfaceToObjCast) CastNode->SetPurity(true);
		CastNode->NodePosX = X;
		CastNode->NodePosY = Y;
		CastNode->AllocateDefaultPins();
		Graph->AddNode(CastNode, false, false);

		for (UEdGraphPin* Pin : CastNode->Pins)
		{
			if (Pin->Direction == EGPD_Input && Pin->PinType.PinCategory != UEdGraphSchema_K2::PC_Exec)
			{
				Pin->PinType.PinCategory = Expr->Token == EX_MetaCast ? UEdGraphSchema_K2::PC_Class : UEdGraphSchema_K2::PC_Object;
				Pin->PinType.PinSubCategoryObject = UObject::StaticClass();
				break;
			}
		}

		if (Expr->Children.Num() > 0 && Expr->Children[0])
		{
			int32 SubX = X - 200;
			int32 SubY = Y;
			UEdGraphPin* InputObj = ResolveDataExpr(Graph, Expr->Children[0], SubX, SubY);
			for (UEdGraphPin* Pin : CastNode->Pins)
			{
				if (Pin->Direction == EGPD_Input && Pin->PinType.PinCategory != UEdGraphSchema_K2::PC_Exec)
				{
					ForceConnect(InputObj, Pin);
					break;
				}
			}
		}

		X -= NodeSpacingX / 2;

		for (UEdGraphPin* Pin : CastNode->Pins)
		{
			if (Pin->Direction == EGPD_Output && Pin->PinType.PinCategory != UEdGraphSchema_K2::PC_Exec)
			{
				return Pin;
			}
		}
		return nullptr;
	}

	case EX_Cast:
	{

		if (Expr->Children.Num() == 0 || !Expr->Children[0]) return nullptr;

		uint8 ConversionType = Expr->ByteValue;
		TSharedPtr<FCASDecompiledExpr> ChildExpr = Expr->Children[0];

		const uint8 CST_ObjectToBool = 0x47;
		const uint8 CST_InterfaceToBool = 0x49;

		if (ConversionType == CST_ObjectToBool || ConversionType == CST_InterfaceToBool)
		{
			int32 SubX = X;
			int32 SubY = Y;
			UEdGraphPin* ChildPin = ResolveDataExpr(Graph, ChildExpr, SubX, SubY);
			if (ChildPin)
			{
				UEdGraphNode* OwnerNode = ChildPin->GetOwningNode();
				if (OwnerNode)
				{

					for (UEdGraphPin* Pin : OwnerNode->Pins)
					{
						if (Pin->Direction == EGPD_Output &&
							Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Boolean)
						{
							return Pin;
						}
					}
				}
			}
			return nullptr;
		}

		int32 SubX = X;
		int32 SubY = Y;
		return ResolveDataExpr(Graph, ChildExpr, SubX, SubY);
	}

	case EX_SkipOffsetConst:
	{

		return nullptr;
	}

	case EX_ClassContext:
    {
        if (!Expr->ContextObject || UE427IsEmpty(Expr->Children) || !Expr->Children.Last()->PropertyRef) return nullptr;
        auto Defaults = NewObject<UK2Node_GetClassDefaults>(Graph);
        Defaults->CreateNewGuid(); Defaults->NodePosX = X; Defaults->NodePosY = Y;
        Graph->AddNode(Defaults, false, false); Defaults->AllocateDefaultPins();
        int32 ClassX = X - NodeSpacingX, ClassY = Y;
        auto ClassPin = ResolveDataExpr(Graph, Expr->ContextObject, ClassX, ClassY);
        if (!TryConnect(ClassPin, Defaults->FindClassPin())) return nullptr;
        Defaults->PinConnectionListChanged(Defaults->FindClassPin());
        const FName Name = Expr->Children.Last()->PropertyRef->GetFName();
        if (auto Options = FindFProperty<FArrayProperty>(Defaults->GetClass(), TEXT("ShowPinForProperties")))
            for (auto& Optional : *Options->ContainerPtrToValuePtr<TArray<FOptionalPinFromProperty>>(Defaults)) if (Optional.PropertyName == Name) Optional.bShowPin = true;
        Defaults->ReconstructNode();
        auto Output = Defaults->FindPin(Name, EGPD_Output);
        if (!Output) Warn(FString::Printf(TEXT("Cannot recover class default property %s"), *Name.ToString()));
        return Output;
    }
	case EX_Context:
	case EX_Context_FailSilent:
	{
		if (Expr->Children.Num() > 0 && Expr->Children[0])
		{
			TSharedPtr<FCASDecompiledExpr> InnerExpr = Expr->Children[0];

			if (InnerExpr->ContextObject && Expr->ContextObject
				&& InnerExpr->ContextObject.Get() != Expr->ContextObject.Get())
			{
				bool bWouldCycle = false;
				for (TSharedPtr<FCASDecompiledExpr> Walk = Expr->ContextObject; Walk; Walk = Walk->ContextObject)
				{
					if (Walk.Get() == InnerExpr->ContextObject.Get())
					{
						bWouldCycle = true;
						break;
					}
				}
				if (!bWouldCycle)
				{
					InnerExpr->ContextObject->ContextObject = Expr->ContextObject;
				}
			}
			else if (!InnerExpr->ContextObject)
			{
				InnerExpr->ContextObject = Expr->ContextObject;
			}

			if (InnerExpr->ContextObject && Expr->FieldRef)
			{
				if (!InnerExpr->ContextObject->FieldRef)
				{
					InnerExpr->ContextObject->FieldRef = Expr->FieldRef;
				}
			}

			Expr = InnerExpr; 
			continue;
		}
		return nullptr;
	}

	case EX_ArrayGetByRef:
	{

		if (Expr->Children.Num() < 2 || !Expr->Children[0] || !Expr->Children[1])
		{
			return nullptr;
		}

		int32 ArrX = X - 200;
		int32 ArrY = Y + DataNodeOffsetY;
		UEdGraphPin* ArrPin = ResolveDataExpr(Graph, Expr->Children[0], ArrX, ArrY);
		if (!ArrPin)
		{
			Warn(TEXT("EX_ArrayGetByRef: Could not resolve array expression"));
			return nullptr;
		}

		FEdGraphPinType ElemType;
		bool bHasElemType = false;
		if (ArrPin->PinType.ContainerType == EPinContainerType::Array)
		{
			ElemType = ArrPin->PinType;
			ElemType.ContainerType = EPinContainerType::None;
			ElemType.bIsReference = false;
			bHasElemType = true;
		}
		else if (Expr->Children[0]->PropertyRef)
		{

			FArrayProperty* ArrayProp = CastField<FArrayProperty>(Expr->Children[0]->PropertyRef);
			if (ArrayProp && ArrayProp->Inner)
			{
				GetDefault<UEdGraphSchema_K2>()->ConvertPropertyToPinType(ArrayProp->Inner, ElemType);
				bHasElemType = true;
			}
		}

		UK2Node_GetArrayItem* GetNode = NewObject<UK2Node_GetArrayItem>(Graph);
		GetNode->CreateNewGuid();
		GetNode->NodePosX = X;
		GetNode->NodePosY = Y;
		GetNode->AllocateDefaultPins();
		Graph->AddNode(GetNode, false, false);

		if (bHasElemType)
		{
			UEdGraphPin* ArrayPin = GetNode->GetTargetArrayPin();
			if (ArrayPin)
			{
				ArrayPin->PinType = ElemType;
				ArrayPin->PinType.ContainerType = EPinContainerType::Array;
				ArrayPin->PinType.bIsReference = false;
			}
			UEdGraphPin* ResultPin = GetNode->GetResultPin();
			if (ResultPin)
			{
				ResultPin->PinType = ElemType;
				ResultPin->PinType.ContainerType = EPinContainerType::None;
				ResultPin->PinType.bIsConst = false;
			}
		}

		UEdGraphPin* TargetArrayPin = GetNode->GetTargetArrayPin();
		if (ArrPin && TargetArrayPin)
		{
			ForceConnect(ArrPin, TargetArrayPin);
		}

		int32 IdxX = X - 200;
		int32 IdxY = Y + DataNodeOffsetY * 2;
		UEdGraphPin* IdxPin = ResolveDataExpr(Graph, Expr->Children[1], IdxX, IdxY);
		UEdGraphPin* IndexPin = GetNode->GetIndexPin();
		if (IdxPin && IndexPin)
		{
			ForceConnect(IdxPin, IndexPin);
		}

		UEdGraphPin* OutputPin = GetNode->GetResultPin();
		X -= NodeSpacingX;
		return OutputPin;
	}

	case EX_StructMemberContext:
	{
		FProperty* MemberProp = Expr->PropertyRef;
		if (!MemberProp || Expr->Children.Num() == 0 || !Expr->Children[0])
		{
			return nullptr;
		}

		UScriptStruct* StructType = Cast<UScriptStruct>(MemberProp->GetOwnerStruct());
		if (!StructType)
		{

			Expr = Expr->Children[0];
			continue;
		}

		int32 BreakX = X - NodeSpacingX;
		int32 BreakY = Y;
		UEdGraphPin* StructPin = ResolveDataExpr(Graph, Expr->Children[0], BreakX, BreakY);
		const bool bDeferredStruct = !StructPin && Expr->Children[0]->PropertyRef &&
			(Expr->Children[0]->Token == EX_LocalVariable || Expr->Children[0]->Token == EX_LocalOutVariable);
		if (!StructPin && !bDeferredStruct)
		{
			Warn(FString::Printf(TEXT("Break Struct: could not resolve struct source for member '%s' on %s"),
				*MemberProp->GetName(), *StructType->GetName()));
			return nullptr;
		}

		if (UScriptStruct* SourceStruct = StructPin ? Cast<UScriptStruct>(StructPin->PinType.PinSubCategoryObject.Get()) : nullptr)
			if (SourceStruct->IsChildOf(StructType)) StructType = SourceStruct;
		if (StructType->HasMetaData(TEXT("HasNativeBreak")))
		{
			FString ClassPath, FunctionName;
			StructType->GetMetaData(TEXT("HasNativeBreak")).Split(TEXT("."), &ClassPath, &FunctionName, ESearchCase::CaseSensitive, ESearchDir::FromEnd);
			if (!ClassPath.StartsWith(TEXT("/Script/"))) ClassPath = TEXT("/Script/") + ClassPath;
			auto NativeClass = FindObject<UClass>(nullptr, *ClassPath);
			auto Function = NativeClass ? NativeClass->FindFunctionByName(*FunctionName) : nullptr;
			if (Function)
			{
				auto Call = NewObject<UK2Node_CallFunction>(Graph);
				Call->CreateNewGuid();
				Call->SetFromFunction(Function);
				Call->NodePosX = X;
				Call->NodePosY = Y;
				Graph->AddNode(Call, false, false);
				Call->AllocateDefaultPins();
				for (auto Input : Call->Pins)
					if (Input->Direction == EGPD_Input && Input->PinType.PinSubCategoryObject.Get() == StructType)
					{
						if (bDeferredStruct) DeferredDataWires.Emplace(Input, Expr->Children[0]->PropertyRef->GetFName());
						else if (!TryConnect(StructPin, Input)) break;
						if (auto Output = Call->FindPin(MemberProp->GetFName(), EGPD_Output)) return Output;
					}
			}
			Warn(FString::Printf(TEXT("Cannot reconstruct native break for %s.%s"), *StructType->GetName(), *MemberProp->GetName()));
			return nullptr;
		}

		UK2Node_BreakStruct* BreakNode = NewObject<UK2Node_BreakStruct>(Graph);
		BreakNode->CreateNewGuid();
		BreakNode->StructType = StructType;
		BreakNode->bMadeAfterOverridePinRemoval = true;
		BreakNode->NodePosX = X;
		BreakNode->NodePosY = Y;
		BreakNode->AllocateDefaultPins();
		Graph->AddNode(BreakNode, false, false);
		for (auto& OptionalPin : BreakNode->ShowPinForProperties)
			if (OptionalPin.PropertyName == MemberProp->GetFName()) OptionalPin.bShowPin = true;
		BreakNode->ReconstructNode();

		UEdGraphPin* InputPin = nullptr;
		for (UEdGraphPin* Pin : BreakNode->Pins)
		{
			if (Pin && Pin->Direction == EGPD_Input)
			{
				InputPin = Pin;
				break;
			}
		}
		if (InputPin && bDeferredStruct) DeferredDataWires.Emplace(InputPin, Expr->Children[0]->PropertyRef->GetFName());
		else if (!InputPin || !TryConnect(StructPin, InputPin))
		{
			Warn(FString::Printf(TEXT("Cannot connect struct source %s.%s (%s %s) to break struct %s"), *GetNameSafe(StructPin ? StructPin->GetOwningNode() : nullptr), StructPin ? *StructPin->PinName.ToString() : TEXT("missing"), StructPin ? *StructPin->PinType.PinCategory.ToString() : TEXT("missing"), StructPin ? *GetPathNameSafe(StructPin->PinType.PinSubCategoryObject.Get()) : TEXT("missing"), *StructType->GetName()));
			return nullptr;
		}

		FName MemberName = MemberProp->GetFName();
		for (UEdGraphPin* Pin : BreakNode->Pins)
		{
			if (Pin && Pin->Direction == EGPD_Output && Pin->PinName == MemberName)
			{
				return Pin;
			}
		}

		for (UEdGraphPin* Pin : BreakNode->Pins)
		{
			if (Pin && Pin->Direction == EGPD_Output &&
				Pin->PinName.ToString().Equals(MemberName.ToString(), ESearchCase::IgnoreCase))
			{
				return Pin;
			}
		}

		Warn(FString::Printf(TEXT("Break Struct: could not find output pin '%s' on %s (has %d pins)"),
			*MemberName.ToString(), *StructType->GetName(), BreakNode->Pins.Num()));

		for (UEdGraphPin* Pin : BreakNode->Pins)
		{
			if (Pin && Pin->Direction == EGPD_Output)
			{
				return Pin;
			}
		}

		return nullptr;
	}

	case EX_VirtualFunction:
	case EX_LocalVirtualFunction:
	{

		UFunction* ResolvedFunc = Expr->FunctionRef;
		FName FuncName = Expr->NameValue;

		if (Expr->ContextObject)
		{
			TSharedPtr<FCASDecompiledExpr> CtxExpr = Expr->ContextObject;
			while (CtxExpr && !ResolvedFunc)
			{

				if (CtxExpr->FieldRef)
				{
					FProperty* Prop = CastField<FProperty>(CtxExpr->FieldRef);
					if (Prop)
					{
						FObjectPropertyBase* ObjProp = CastField<FObjectPropertyBase>(Prop);
						if (ObjProp && ObjProp->PropertyClass)
							ResolvedFunc = ObjProp->PropertyClass->FindFunctionByName(FuncName);
						if (!ResolvedFunc)
						{
							FInterfaceProperty* IntProp = CastField<FInterfaceProperty>(Prop);
							if (IntProp && IntProp->InterfaceClass)
								ResolvedFunc = IntProp->InterfaceClass->FindFunctionByName(FuncName);
						}
					}
				}

				if (!ResolvedFunc && CtxExpr->Token == EX_InterfaceContext && CtxExpr->Children.Num() > 0)
				{
					TSharedPtr<FCASDecompiledExpr> InnerExpr = CtxExpr->Children[0];
					if (InnerExpr && InnerExpr->PropertyRef)
					{
						FInterfaceProperty* IntProp = CastField<FInterfaceProperty>(InnerExpr->PropertyRef);
						if (IntProp && IntProp->InterfaceClass)
							ResolvedFunc = IntProp->InterfaceClass->FindFunctionByName(FuncName);
						if (!ResolvedFunc)
						{
							FObjectPropertyBase* ObjProp = CastField<FObjectPropertyBase>(InnerExpr->PropertyRef);
							if (ObjProp && ObjProp->PropertyClass)
								ResolvedFunc = ObjProp->PropertyClass->FindFunctionByName(FuncName);
						}
					}
				}

				if (!ResolvedFunc && CtxExpr->PropertyRef)
				{
					FObjectPropertyBase* ObjProp = CastField<FObjectPropertyBase>(CtxExpr->PropertyRef);
					if (ObjProp && ObjProp->PropertyClass)
						ResolvedFunc = ObjProp->PropertyClass->FindFunctionByName(FuncName);
					if (!ResolvedFunc)
					{
						FInterfaceProperty* IntProp = CastField<FInterfaceProperty>(CtxExpr->PropertyRef);
						if (IntProp && IntProp->InterfaceClass)
							ResolvedFunc = IntProp->InterfaceClass->FindFunctionByName(FuncName);
					}
				}

				if (!ResolvedFunc && CtxExpr->FunctionRef)
				{
					FProperty* RetProp = CtxExpr->FunctionRef->GetReturnProperty();
					if (RetProp)
					{
						FObjectPropertyBase* ObjProp = CastField<FObjectPropertyBase>(RetProp);
						if (ObjProp && ObjProp->PropertyClass)
							ResolvedFunc = ObjProp->PropertyClass->FindFunctionByName(FuncName);
					}
				}

				if (!ResolvedFunc && CtxExpr->ObjectRef)
				{
					UClass* ObjClass = CtxExpr->ObjectRef->GetClass();
					if (ObjClass)
						ResolvedFunc = ObjClass->FindFunctionByName(FuncName);
				}
				if (CtxExpr->ContextObject)
					CtxExpr = CtxExpr->ContextObject;
				else if (CtxExpr->Children.Num() > 0)
					CtxExpr = CtxExpr->Children[0];
				else
					break;
			}
		}

		if (!ResolvedFunc && CurrentOriginalClass)
		{
			ResolvedFunc = CurrentOriginalClass->FindFunctionByName(FuncName);
		}
		if (!ResolvedFunc && Graph->GetOuter())
		{
			UBlueprint* BP = Cast<UBlueprint>(Graph->GetOuter());
			if (BP && BP->ParentClass)
			{
				ResolvedFunc = BP->ParentClass->FindFunctionByName(FuncName);
			}
		}
		if (!ResolvedFunc)
		{
			ResolvedFunc = UKismetSystemLibrary::StaticClass()->FindFunctionByName(FuncName);
		}
		if (!ResolvedFunc)
		{
			ResolvedFunc = UKismetMathLibrary::StaticClass()->FindFunctionByName(FuncName);
		}

		if (ResolvedFunc)
		{
			Expr->FunctionRef = ResolvedFunc;
			Expr->Token = EX_FinalFunction; 
			continue; 
		}

		Warn(FString::Printf(TEXT("Could not resolve virtual function %s for data expression"), *Expr->NameValue.ToString()));
		return nullptr;
	}

	case EX_VectorConst:
	{
		auto Literal = MakeShared<FCASDecompiledExpr>();
		Literal->Token = EX_StructConst;
		Literal->StructRef = TBaseStructure<FVector>::Get();
		for (double Component : {Expr->VectorValue.X, Expr->VectorValue.Y, Expr->VectorValue.Z})
		{
			auto Value = MakeShared<FCASDecompiledExpr>();
			Value->Token = EX_DoubleConst;
			Value->FloatValue = Component;
			Literal->Children.Add(Value);
		}
		Expr = Literal;
		continue;
	}

	case EX_StructConst:
	{
		if (!Expr->StructRef) return nullptr;

		FString StructName = Expr->StructRef->GetName();
		if (StructName == TEXT("GameplayTagContainer") && Expr->Children.Num() == 1 && Expr->Children[0]->Token == EX_ArrayConst)
		{
			auto Library = FindObject<UClass>(nullptr, TEXT("/Script/GameplayTags.BlueprintGameplayTagLibrary"));
			auto Function = Library ? Library->FindFunctionByName(TEXT("MakeGameplayTagContainerFromArray")) : nullptr;
			auto Parameter = Function ? FindFProperty<FArrayProperty>(Function, TEXT("GameplayTags")) : nullptr;
			if (!Parameter) { Warn(TEXT("Cannot resolve MakeGameplayTagContainerFromArray")); return nullptr; }
			auto Call = MakeShared<FCASDecompiledExpr>();
			Call->Token = EX_CallMath;
			Call->FunctionRef = Function;
			Call->NameValue = Function->GetFName();
			Call->Children = Expr->Children;
			Call->Children[0]->PropertyRef = Parameter->Inner;
			Expr = Call;
			continue;
		}
		if (StructName == TEXT("GameplayTag") && Expr->Children.Num() == 1 && Expr->Children[0]->Token == EX_NameConst)
		{
			auto Library = FindObject<UClass>(nullptr, TEXT("/Script/GameplayTags.BlueprintGameplayTagLibrary"));
			auto Function = Library ? Library->FindFunctionByName(TEXT("MakeLiteralGameplayTag")) : nullptr;
			if (!Function) { Warn(TEXT("Cannot resolve MakeLiteralGameplayTag")); return nullptr; }
			auto Literal = NewObject<UK2Node_CallFunction>(Graph);
			Literal->CreateNewGuid();
			Literal->SetFromFunction(Function);
			Literal->NodePosX = X;
			Literal->NodePosY = Y;
			Graph->AddNode(Literal, false, false);
			Literal->AllocateDefaultPins();
			Literal->FindPinChecked(TEXT("Value"))->DefaultValue = FString::Printf(TEXT("(TagName=\"%s\")"), *Expr->Children[0]->NameValue.ToString());
			X -= NodeSpacingX;
			return Literal->FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue);
		}

		if (StructName == TEXT("LatentActionInfo"))
		{
			return nullptr;
		}

		FName MakeFuncName = NAME_None;
		UClass* MakeLibrary = UKismetMathLibrary::StaticClass();

		if (StructName == TEXT("Transform"))
		{
			MakeFuncName = TEXT("MakeTransform");
		}
		else if (StructName == TEXT("Vector"))
		{
			MakeFuncName = TEXT("MakeVector");
		}
		else if (StructName == TEXT("Rotator"))
		{
			MakeFuncName = TEXT("MakeRotator");
		}
		else if (StructName == TEXT("Vector2D"))
		{
			MakeFuncName = TEXT("MakeVector2D");
		}
		else if (StructName == TEXT("LinearColor"))
		{
			MakeFuncName = TEXT("MakeColor");
		}

		if (MakeFuncName != NAME_None)
		{
			UFunction* MakeFunc = MakeLibrary->FindFunctionByName(MakeFuncName);
			if (MakeFunc)
			{
				UK2Node_CallFunction* MakeNode = NewObject<UK2Node_CallFunction>(Graph);
				MakeNode->CreateNewGuid();
				MakeNode->SetFromFunction(MakeFunc);
				MakeNode->NodePosX = X;
				MakeNode->NodePosY = Y;
				MakeNode->AllocateDefaultPins();
				Graph->AddNode(MakeNode, false, false);

				int32 ChildIdx = 0;
				int32 ParamIdx = 0;
				for (TFieldIterator<FProperty> It(MakeFunc); It; ++It)
				{
					FProperty* Param = *It;
					if (!Param->HasAnyPropertyFlags(CPF_Parm) || Param->HasAnyPropertyFlags(CPF_ReturnParm))
						continue;

					if (ChildIdx < Expr->Children.Num() && Expr->Children[ChildIdx])
					{
						TSharedPtr<FCASDecompiledExpr> ChildExpr = Expr->Children[ChildIdx];
						UEdGraphPin* ParamPin = FindDataPin(MakeNode, EGPD_Input, Param->GetFName());
						if (ParamPin)
						{
							if (ChildExpr->IsLiteral())
							{

								bool bEnumResolved = false;
								if ((ChildExpr->Token == EX_ByteConst || ChildExpr->Token == EX_IntConst ||
									ChildExpr->Token == EX_IntConstByte || ChildExpr->Token == EX_IntZero || ChildExpr->Token == EX_IntOne) &&
									ParamPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Byte &&
									ParamPin->PinType.PinSubCategoryObject.IsValid())
								{
									UEnum* PinEnum = Cast<UEnum>(ParamPin->PinType.PinSubCategoryObject.Get());
									if (PinEnum)
									{
										int64 EnumVal = (ChildExpr->Token == EX_ByteConst) ? ChildExpr->ByteValue : ChildExpr->IntValue;
										FString EnumName = PinEnum->GetNameStringByValue(EnumVal);
										if (!UE427IsEmpty(EnumName))
										{
											ParamPin->DefaultValue = EnumName;
											bEnumResolved = true;
										}
									}
								}
								if (!bEnumResolved)
								{
									FString Val = ChildExpr->GetLiteralAsString();
									if (!UE427IsEmpty(Val))
									{
										ParamPin->DefaultValue = Val;
									}
								}
							}
							else
							{

								int32 SubX = X - 250;
								int32 SubY = Y + ChildIdx * 50;
								UEdGraphPin* ChildPin = ResolveDataExpr(Graph, ChildExpr, SubX, SubY);
								ForceConnect(ChildPin, ParamPin);
							}
						}
					}
					ChildIdx++;
					ParamIdx++;
				}

				X -= NodeSpacingX;

				UEdGraphPin* RetPin = FindDataPin(MakeNode, EGPD_Output, UEdGraphSchema_K2::PN_ReturnValue);
				return RetPin ? RetPin : FindFirstDataOutputPin(MakeNode);
			}
		}

		auto MakeNode = NewObject<UK2Node_MakeStruct>(Graph);
		MakeNode->CreateNewGuid();
		MakeNode->StructType = Expr->StructRef;
		MakeNode->NodePosX = X;
		MakeNode->NodePosY = Y;
		Graph->AddNode(MakeNode, false, false);
		MakeNode->AllocateDefaultPins();
		int32 ChildIndex = 0;
		for (TFieldIterator<FProperty> It(Expr->StructRef); It; ++It)
		{
			if (It->HasAnyPropertyFlags(CPF_EditorOnly)) continue;
			for (int32 Index = 0; Index < It->ArrayDim && ChildIndex < Expr->Children.Num(); ++Index, ++ChildIndex)
			{
				auto Child = Expr->Children[ChildIndex];
				auto Pin = MakeNode->FindPin(It->GetFName(), EGPD_Input);
				if (!Pin || !Child) continue;
				if (Child->IsLiteral() && Child->Token != EX_Self)
				{
					if (!ApplyTempLiteralToPin(Child, Pin)) Pin->DefaultValue = Child->GetLiteralAsString();
				}
				else
				{
					int32 ChildX = X - 250, ChildY = Y + ChildIndex * 50;
					ForceConnect(ResolveDataExpr(Graph, Child, ChildX, ChildY), Pin);
				}
			}
		}
		X -= NodeSpacingX;
		return FindFirstDataOutputPin(MakeNode);
	}

	case EX_ArrayConst:
	{
		int32 NumElements = Expr->IntValue;

		UK2Node_MakeArray* MakeArrayNode = NewObject<UK2Node_MakeArray>(Graph);
		MakeArrayNode->CreateNewGuid();
		MakeArrayNode->NumInputs = NumElements;
		MakeArrayNode->NodePosX = X;
		MakeArrayNode->NodePosY = Y;
		MakeArrayNode->AllocateDefaultPins();
		Graph->AddNode(MakeArrayNode, false, false);

		if (Expr->PropertyRef)
		{
			FEdGraphPinType ElemPinType;
			GetDefault<UEdGraphSchema_K2>()->ConvertPropertyToPinType(Expr->PropertyRef, ElemPinType);

			for (UEdGraphPin* Pin : MakeArrayNode->Pins)
			{
				if (Pin->Direction == EGPD_Output)
				{
					Pin->PinType = ElemPinType;
					Pin->PinType.ContainerType = EPinContainerType::Array;
				}
				else if (Pin->Direction == EGPD_Input &&
					Pin->PinType.PinCategory != UEdGraphSchema_K2::PC_Exec)
				{
					Pin->PinType = ElemPinType;
				}
			}
		}

		int32 ElemX = X - 250;
		int32 ElemY = Y;
		for (int32 i = 0; i < NumElements && i < Expr->Children.Num(); i++)
		{
			TSharedPtr<FCASDecompiledExpr> ElemExpr = Expr->Children[i];
			if (!ElemExpr) continue;

			UEdGraphPin* ElemPin = nullptr;
			int32 DataIdx = 0;
			for (UEdGraphPin* Pin : MakeArrayNode->Pins)
			{
				if (Pin->Direction == EGPD_Input &&
					Pin->PinType.PinCategory != UEdGraphSchema_K2::PC_Exec)
				{
					if (DataIdx == i) { ElemPin = Pin; break; }
					DataIdx++;
				}
			}

			if (ElemPin)
			{

				if (ElemExpr->Token == EX_Self)
				{
					UK2Node_Self* SelfNode = NewObject<UK2Node_Self>(Graph);
					SelfNode->CreateNewGuid();
					SelfNode->NodePosX = ElemX;
					SelfNode->NodePosY = ElemY;
					SelfNode->AllocateDefaultPins();
					Graph->AddNode(SelfNode, false, false);
					UEdGraphPin* SelfOut = FindFirstDataOutputPin(SelfNode);
					ForceConnect(SelfOut, ElemPin);
					ElemY += NodeSpacingY / 3;
				}
				else if (ElemExpr->Token == EX_NoObject)
				{
					ElemPin->DefaultValue = TEXT("");
				}
				else if (ElemExpr->IsLiteral())
				{
					if ((ElemExpr->Token == EX_ByteConst || ElemExpr->Token == EX_IntConst ||
						ElemExpr->Token == EX_IntConstByte) &&
						ElemPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Byte &&
						ElemPin->PinType.PinSubCategoryObject.IsValid())
					{
						UEnum* PinEnum = Cast<UEnum>(ElemPin->PinType.PinSubCategoryObject.Get());
						if (PinEnum)
						{
							int64 Val = (ElemExpr->Token == EX_ByteConst) ? ElemExpr->ByteValue : ElemExpr->IntValue;
							FString EnumName = PinEnum->GetNameStringByValue(Val);
							ElemPin->DefaultValue = UE427IsEmpty(EnumName) ? ElemExpr->GetLiteralAsString() : EnumName;
						}
						else
						{
							ElemPin->DefaultValue = ElemExpr->GetLiteralAsString();
						}
					}
					else if (ElemExpr->Token == EX_ObjectConst)
					{
						if (ElemExpr->ObjectRef)
						{
							ElemPin->DefaultObject = ElemExpr->ObjectRef;
						}
					}
					else
					{
						ElemPin->DefaultValue = ElemExpr->GetLiteralAsString();
					}
				}
				else
				{
					UEdGraphPin* ElemOut = ResolveDataExpr(Graph, ElemExpr, ElemX, ElemY);
					ForceConnect(ElemOut, ElemPin);
					ElemY += NodeSpacingY / 3;
				}
			}
		}

		X -= NodeSpacingX;
		return FindFirstDataOutputPin(MakeArrayNode);
	}

	case EX_InterfaceContext:
	{

		if (Expr->Children.Num() > 0 && Expr->Children[0])
		{
			Expr = Expr->Children[0]; 
			continue;
		}
		return nullptr;
	}

	case EX_SwitchValue:
	{

		int32 NumCases = Expr->IntValue;
		int32 ExpectedChildren = 1 + NumCases * 2 + 1;
		if (Expr->Children.Num() < ExpectedChildren)
		{
			Warn(FString::Printf(TEXT("SwitchValue: expected %d children, got %d"),
				ExpectedChildren, Expr->Children.Num()));
			return nullptr;
		}

		TSharedPtr<FCASDecompiledExpr> IndexExpr = Expr->Children[0];

		FEdGraphPinType ValuePinType;
		ValuePinType.PinCategory = UEdGraphSchema_K2::PC_Wildcard;
		{

			auto InferTypeFromLiteralToken = [](EExprToken Token, TSharedPtr<FCASDecompiledExpr> LitExpr, FEdGraphPinType& OutType) -> bool
			{
				switch (Token)
				{
				case EX_StructConst:
					if (!LitExpr || !LitExpr->StructRef) return false;
					OutType.PinCategory = UEdGraphSchema_K2::PC_Struct;
					OutType.PinSubCategoryObject = LitExpr->StructRef;
					return true;
				case EX_NameConst:
					OutType.PinCategory = UEdGraphSchema_K2::PC_Name;
					return true;
				case EX_StringConst:
				case EX_UnicodeStringConst:
					OutType.PinCategory = UEdGraphSchema_K2::PC_String;
					return true;
				case EX_IntConst:
				case EX_IntConstByte:
				case EX_IntZero:
				case EX_IntOne:
					OutType.PinCategory = UEdGraphSchema_K2::PC_Int;
					return true;
				case EX_Int64Const:
				case EX_UInt64Const:
					OutType.PinCategory = UEdGraphSchema_K2::PC_Int64;
					return true;
				case EX_FloatConst:
					OutType.PinCategory = UEdGraphSchema_K2::PC_Float;
					return true;
				case EX_True:
				case EX_False:
					OutType.PinCategory = UEdGraphSchema_K2::PC_Boolean;
					return true;
				case EX_ByteConst:
					OutType.PinCategory = UEdGraphSchema_K2::PC_Byte;
					return true;
				case EX_TextConst:
					OutType.PinCategory = UEdGraphSchema_K2::PC_Text;
					return true;
				case EX_ObjectConst:
				case EX_NoObject:
					OutType.PinCategory = UEdGraphSchema_K2::PC_Object;
					if (LitExpr && LitExpr->ObjectRef)
					{
						OutType.PinSubCategoryObject = LitExpr->ObjectRef->GetClass();
					}
					else
					{
						OutType.PinSubCategoryObject = UObject::StaticClass();
					}
					return true;
				case EX_VectorConst:
					OutType.PinCategory = UEdGraphSchema_K2::PC_Struct;
					OutType.PinSubCategoryObject = TBaseStructure<FVector>::Get();
					return true;
				case EX_RotationConst:
					OutType.PinCategory = UEdGraphSchema_K2::PC_Struct;
					OutType.PinSubCategoryObject = TBaseStructure<FRotator>::Get();
					return true;
				default:
					return false;
				}
			};

			TArray<FEdGraphPinType> CaseResultTypes;
			for (int32 ci = 0; ci <= NumCases; ci++)
			{
				TSharedPtr<FCASDecompiledExpr> CaseResult = ci == NumCases ? Expr->Children.Last() : Expr->Children[2 + ci * 2];
				if (!CaseResult) continue;
				while ((CaseResult->Token == EX_Context || CaseResult->Token == EX_Context_FailSilent || CaseResult->Token == EX_ClassContext) && CaseResult->Children.Num() == 1)
					CaseResult = CaseResult->Children[0];

				FEdGraphPinType CaseType;
				CaseType.PinCategory = UEdGraphSchema_K2::PC_Wildcard;

				if (CaseResult->PropertyRef)
				{
					GetDefault<UEdGraphSchema_K2>()->ConvertPropertyToPinType(CaseResult->PropertyRef, CaseType);
				}

				else if (CaseResult->IsLiteral())
				{
					InferTypeFromLiteralToken(CaseResult->Token, CaseResult, CaseType);
				}

				else if (CaseResult->Token == EX_LocalVariable && CaseResult->PropertyRef)
				{
					FName VarName = CaseResult->PropertyRef->GetFName();
					TSharedPtr<FCASDecompiledExpr>* StoredLit = TempLiteralMap.Find(VarName);
					if (StoredLit && (*StoredLit)->IsLiteral())
					{
						InferTypeFromLiteralToken((*StoredLit)->Token, *StoredLit, CaseType);
					}
					if (CaseType.PinCategory == UEdGraphSchema_K2::PC_Wildcard)
					{
						GetDefault<UEdGraphSchema_K2>()->ConvertPropertyToPinType(CaseResult->PropertyRef, CaseType);
					}
				}

				if (CaseType.PinCategory != UEdGraphSchema_K2::PC_Wildcard)
				{
					CaseResultTypes.Add(CaseType);
				}
			}

			if (CaseResultTypes.Num() > 0)
			{
				ValuePinType = CaseResultTypes[0];

				bool bAllSame = true;
				for (int32 i = 1; i < CaseResultTypes.Num(); i++)
				{
					if (CaseResultTypes[i].PinCategory != ValuePinType.PinCategory ||
						CaseResultTypes[i].PinSubCategoryObject != ValuePinType.PinSubCategoryObject)
					{
						bAllSame = false;
						break;
					}
				}

				if (!bAllSame)
				{
					bool bAllObjects = true;
					for (const FEdGraphPinType& CaseType : CaseResultTypes)
					{
						if (CaseType.PinCategory != UEdGraphSchema_K2::PC_Object &&
							CaseType.PinCategory != UEdGraphSchema_K2::PC_Interface &&
							CaseType.PinCategory != UEdGraphSchema_K2::PC_SoftObject)
						{
							bAllObjects = false;
							break;
						}
					}

					if (bAllObjects)
					{
						ValuePinType.PinCategory = UEdGraphSchema_K2::PC_Object;
						ValuePinType.PinSubCategoryObject = UObject::StaticClass();
					}
				}
			}
		}

		FEdGraphPinType IndexPinType;
		IndexPinType.PinCategory = UEdGraphSchema_K2::PC_Wildcard;
		if (IndexExpr && IndexExpr->PropertyRef)
		{
			GetDefault<UEdGraphSchema_K2>()->ConvertPropertyToPinType(IndexExpr->PropertyRef, IndexPinType);
		}

		UK2Node_Select* SelectNode = NewObject<UK2Node_Select>(Graph);
		SelectNode->CreateNewGuid();
		SelectNode->NodePosX = X;
		SelectNode->NodePosY = Y;

		if (NumCases > 2)
		{
			FIntProperty* NumOptionPinsProp = CastField<FIntProperty>(
				UK2Node_Select::StaticClass()->FindPropertyByName(TEXT("NumOptionPins")));
			if (NumOptionPinsProp)
			{
				NumOptionPinsProp->SetPropertyValue_InContainer(SelectNode, NumCases);
			}
			else
			{
				Warn(TEXT("Could not find NumOptionPins property on UK2Node_Select — extra option pins may not survive save/reload"));
			}
		}

		if (IndexPinType.PinCategory != UEdGraphSchema_K2::PC_Wildcard)
		{
			FStructProperty* IndexPinTypeProp = CastField<FStructProperty>(
				UK2Node_Select::StaticClass()->FindPropertyByName(TEXT("IndexPinType")));
			if (IndexPinTypeProp)
			{
				FEdGraphPinType* MemberPtr = IndexPinTypeProp->ContainerPtrToValuePtr<FEdGraphPinType>(SelectNode);
				if (MemberPtr)
				{
					*MemberPtr = IndexPinType;
				}
			}
		}

		UEnum* IndexEnum = Cast<UEnum>(IndexPinType.PinSubCategoryObject.Get());
		if (IndexEnum) SelectNode->SetEnum(IndexEnum);
		SelectNode->AllocateDefaultPins();
		Graph->AddNode(SelectNode, false, false);

		UEdGraphPin* IndexPin = SelectNode->GetIndexPin();
		if (IndexPin && IndexPinType.PinCategory != UEdGraphSchema_K2::PC_Wildcard)
		{
			IndexPin->PinType = IndexPinType;
		}

		UEdGraphPin* ReturnPin = SelectNode->GetReturnValuePin();
		if (ReturnPin && ValuePinType.PinCategory != UEdGraphSchema_K2::PC_Wildcard)
		{
			ReturnPin->PinType = ValuePinType;
		}

		if (IndexPin)
		{
			int32 IdxX = X - NodeSpacingX / 2;
			int32 IdxY = Y - NodeSpacingY;
			UEdGraphPin* IndexOut = ResolveDataExpr(Graph, IndexExpr, IdxX, IdxY);
			if (IndexOut)
			{
				ForceConnect(IndexOut, IndexPin);
			}
			else
			{

				ApplyTempLiteralToPin(IndexExpr, IndexPin);
			}
		}

		TArray<UEdGraphPin*> SelectOptions;
		SelectNode->GetOptionPins(SelectOptions);
		for (int32 i = 0; i < SelectOptions.Num(); i++)
		{
			TSharedPtr<FCASDecompiledExpr> CaseResult = Expr->Children.Last();
			UEdGraphPin* OptionPin = SelectOptions[i];
			for (int32 CaseIndex = 0; CaseIndex < NumCases; ++CaseIndex)
			{
				const auto& CaseValue = Expr->Children[1 + CaseIndex * 2];
				const int64 Value = CaseValue->Token == EX_ByteConst ? CaseValue->ByteValue : CaseValue->IntValue;
				const FName CasePinName = IndexEnum ? FName(*IndexEnum->GetNameStringByValue(Value)) : FName(*FString::Printf(TEXT("Option %d"), CaseIndex));
				if (OptionPin->PinName == CasePinName) { CaseResult = Expr->Children[2 + CaseIndex * 2]; break; }
			}

			if (OptionPin && CaseResult)
			{

				if (ValuePinType.PinCategory != UEdGraphSchema_K2::PC_Wildcard)
				{
					OptionPin->PinType = ValuePinType;
				}

				if (ApplyTempLiteralToPin(CaseResult, OptionPin))
				{

				}
				else if (CaseResult->IsLiteral())
				{

					bool bEnumResolved = false;
					if ((CaseResult->Token == EX_ByteConst || CaseResult->Token == EX_IntConst ||
						CaseResult->Token == EX_IntConstByte || CaseResult->Token == EX_IntZero || CaseResult->Token == EX_IntOne) &&
						OptionPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Byte &&
						OptionPin->PinType.PinSubCategoryObject.IsValid())
					{
						UEnum* PinEnum = Cast<UEnum>(OptionPin->PinType.PinSubCategoryObject.Get());
						if (PinEnum)
						{
							int64 Val = (CaseResult->Token == EX_ByteConst) ? CaseResult->ByteValue : CaseResult->IntValue;
							FString EnumName = PinEnum->GetNameStringByValue(Val);
							if (!UE427IsEmpty(EnumName))
							{
								OptionPin->DefaultValue = EnumName;
								bEnumResolved = true;
							}
						}
					}
					if (!bEnumResolved)
					{
						OptionPin->DefaultValue = CaseResult->GetLiteralAsString();
					}
				}
				else
				{

					int32 OptX = X - NodeSpacingX;
					int32 OptY = Y + i * (NodeSpacingY / 2);
					UEdGraphPin* ResultOut = ResolveDataExpr(Graph, CaseResult, OptX, OptY);
					if (ResultOut)
					{
						ForceConnect(ResultOut, OptionPin);
					}
				}
			}
		}

		X -= NodeSpacingX;

		{
			FBoolProperty* ReconstructProp = CastField<FBoolProperty>(
				UK2Node_Select::StaticClass()->FindPropertyByName(TEXT("bReconstructNode")));
			if (ReconstructProp)
			{
				ReconstructProp->SetPropertyValue_InContainer(SelectNode, false);
			}
		}

		return ReturnPin;
	}

	default:

		return nullptr;
	}
	} 
}

void FCASGraphBuilder::FixStructPinDefaults(UEdGraphNode* Node)
{
	if (!Node) return;

	for (UEdGraphPin* Pin : Node->Pins)
	{
		if (Pin->Direction != EGPD_Input) continue;
		if (Pin->PinType.PinCategory != UEdGraphSchema_K2::PC_Struct) continue;
		if (Pin->LinkedTo.Num() > 0) continue; 

		FString& DefaultVal = Pin->DefaultValue;
		if (UE427IsEmpty(DefaultVal)) continue;

		if (DefaultVal.Contains(TEXT("Pitch=")) && DefaultVal.Contains(TEXT("Yaw=")) && DefaultVal.Contains(TEXT("Roll=")))
		{

			float Pitch = 0, Yaw = 0, Roll = 0;
			FParse::Value(*DefaultVal, TEXT("Pitch="), Pitch);
			FParse::Value(*DefaultVal, TEXT("Yaw="), Yaw);
			FParse::Value(*DefaultVal, TEXT("Roll="), Roll);
			Pin->DefaultValue = FString::Printf(TEXT("%f,%f,%f"), Pitch, Yaw, Roll);
		}

		else if (DefaultVal.StartsWith(TEXT("(")) && DefaultVal.Contains(TEXT("X=")) &&
			DefaultVal.Contains(TEXT("Y=")) && DefaultVal.Contains(TEXT("Z=")))
		{
			FVector Vec;
			if (Vec.InitFromString(DefaultVal))
			{
				Pin->DefaultValue = FString::Printf(TEXT("%f,%f,%f"), Vec.X, Vec.Y, Vec.Z);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE

