#include "CASJsonGraph.h"
#include "UE427AssetPath.h"
#include "AnimationGraphReconstruction.h"
#include "K2Node_CustomEvent.h"
#include "Animation/AnimBlueprint.h"
#include "K2Node_FunctionEntry.h"
#include "GraphBuilder.h"
#include "MacroReconstruction.h"
#include "AssetGeneration/AssetGeneratorLocalSettings.h"
#include "Dom/JsonObject.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "EdGraphSchema_K2.h"
#include "KismetCompilerMisc.h"
#include "Kismet2/CompilerResultsLog.h"
#include "Toolkit/AssetGeneration/AssetGenerationUtil.h"
#include "Toolkit/ObjectHierarchySerializer.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"
#include "UObject/CoreRedirects.h"
#include "UObject/UObjectIterator.h"
#include "Misc/PackageName.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetTextLibrary.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/SecureHash.h"
#include "Engine/ComponentDelegateBinding.h"
#include "UObject/MetaData.h"
#include "UObject/Package.h"
#include "K2Node_CallFunction.h"
#include "K2Node_FormatText.h"
#include "K2Node_MakeArray.h"
#include "K2Node_MakeMap.h"
#include "K2Node_MakeStruct.h"
#include "K2Node_AssignmentStatement.h"
#include "K2Node_EnumLiteral.h"
#include "Engine/TimelineTemplate.h"
#include "Components/TimelineComponent.h"
#include "BlueprintCompilationManager.h"
#include "K2Node_Timeline.h"
#include "K2Node_VariableGet.h"
#include "K2Node_DynamicCast.h"
#include "Kismet2/BlueprintEditorUtils.h"

DEFINE_LOG_CATEGORY_STATIC(LogCASGraph, Log, All);

namespace
{
using FExpr = FCASDecompiledExpr;

int32 RestoreTimelineInputs(UEdGraph* Graph)
{
	int32 Count = 0;
	const auto Nodes = Graph->Nodes;
	for (auto Node : Nodes)
	{
		auto Call = Cast<UK2Node_CallFunction>(Node);
		auto Function = Call ? Call->GetTargetFunction() : nullptr;
		if (!Function || Function->GetOwnerClass() != UTimelineComponent::StaticClass()) continue;
		const FName Name = Function->GetFName();
		if (Name != TEXT("Play") && Name != TEXT("PlayFromStart") && Name != TEXT("Stop") && Name != TEXT("Reverse") && Name != TEXT("ReverseFromEnd")) continue;
		auto Self = Call->FindPin(UEdGraphSchema_K2::PN_Self, EGPD_Input);
		auto Execute = Call->FindPin(UEdGraphSchema_K2::PN_Execute, EGPD_Input);
		auto Then = Call->FindPin(UEdGraphSchema_K2::PN_Then, EGPD_Output);
		if (!Self || Self->LinkedTo.Num() != 1 || !Execute || UE427IsEmpty(Execute->LinkedTo) || !Then || !UE427IsEmpty(Then->LinkedTo)) continue;
		auto Getter = Cast<UK2Node_VariableGet>(Self->LinkedTo[0]->GetOwningNode());
		if (!Getter || !Getter->VariableReference.IsSelfContext()) continue;
		for (auto Candidate : Nodes)
		{
			auto Timeline = Cast<UK2Node_Timeline>(Candidate);
			if (!Timeline || Timeline->TimelineName != Getter->GetVarName()) continue;
			auto Input = Timeline->FindPin(Name, EGPD_Input);
			if (!Input || Input->PinType.PinCategory != UEdGraphSchema_K2::PC_Exec) break;
			const auto Links = Execute->LinkedTo;
			for (auto Link : Links) Link->MakeLinkTo(Input);
			Call->BreakAllNodeLinks();
			Graph->RemoveNode(Call);
			if (UE427IsEmpty(Getter->GetValuePin()->LinkedTo))
			{
				Getter->BreakAllNodeLinks();
				Graph->RemoveNode(Getter);
			}
			++Count;
			break;
		}
	}
	return Count;
}

int32 RemoveRedundantRealConversions(UEdGraph* Graph)
{
	int32 Removed = 0;
	const auto Nodes = Graph->Nodes;
	for (UEdGraphNode* Node : Nodes)
	{
		auto Call = Cast<UK2Node_CallFunction>(Node);
		auto Function = Call ? Call->GetTargetFunction() : nullptr;
		if (!Function || Function->GetOwnerClass() != UKismetMathLibrary::StaticClass()) continue;
		if (Function->GetFName() != TEXT("Conv_FloatToDouble") && Function->GetFName() != TEXT("Conv_DoubleToFloat")) continue;
		auto Input = Call->FindPin(TEXT("InFloat"), EGPD_Input);
		if (!Input) Input = Call->FindPin(TEXT("InDouble"), EGPD_Input);
		auto Output = Call->FindPin(UEdGraphSchema_K2::PN_ReturnValue, EGPD_Output);
		if (!Input || !Output || Input->LinkedTo.Num() != 1 || UE427IsEmpty(Output->LinkedTo)) continue;
		auto Source = Input->LinkedTo[0];
		if (Source->PinType.PinCategory != UEdGraphSchema_K2::PC_Float || Source->PinType.IsContainer() || Source->PinType.PinSubCategory != Input->PinType.PinSubCategory) continue;
		const auto Targets = Output->LinkedTo;
		bool Compatible = true;
		for (auto Target : Targets)
			if (Target->PinType.PinCategory != UEdGraphSchema_K2::PC_Float || Target->PinType.IsContainer() || Target->PinType.bIsReference || Target->PinType.PinSubCategory != Output->PinType.PinSubCategory) Compatible = false;
		if (!Compatible) continue;
		Output->BreakAllPinLinks();
		Input->BreakAllPinLinks();
		for (auto Target : Targets) Source->MakeLinkTo(Target);
		Graph->RemoveNode(Call);
		++Removed;
	}
	return Removed;
}

bool RenderConstant(const TSharedPtr<FExpr>& Expr, FString& Value)
{
	if (!Expr) return false;
	if (Expr->Token == EX_StructConst)
	{
		if (!Expr->StructRef) return false;
		TArray<FString> Fields;
		int32 Index = 0;
		for (TFieldIterator<FProperty> It(Expr->StructRef); It; ++It)
		{
			if (It->HasAnyPropertyFlags(CPF_EditorOnly)) continue;
			TArray<FString> Elements;
			for (int32 Element = 0; Element < It->ArrayDim; ++Element)
			{
				FString Text;
				if (!Expr->Children.IsValidIndex(Index) || !RenderConstant(Expr->Children[Index++], Text)) return false;
				Elements.Add(Text);
			}
			Fields.Add(It->GetName() + TEXT("=") + (It->ArrayDim == 1 ? Elements[0] : TEXT("(") + FString::Join(Elements, TEXT(",")) + TEXT(")")));
		}
		if (Index != Expr->Children.Num()) return false;
		Value = TEXT("(") + FString::Join(Fields, TEXT(",")) + TEXT(")");
		return true;
	}
	if (Expr->Token == EX_ArrayConst || Expr->Token == EX_SetConst)
	{
		TArray<FString> Values;
		for (const auto& Child : Expr->Children) { FString Text; if (!RenderConstant(Child, Text)) return false; Values.Add(Text); }
		Value = TEXT("(") + FString::Join(Values, TEXT(",")) + TEXT(")");
		return true;
	}
	if (Expr->Token == EX_SoftObjectConst && Expr->Children.Num() == 1) return RenderConstant(Expr->Children[0], Value);
	if (!Expr->IsLiteral()) return false;
	Value = Expr->GetLiteralAsString();
	if (Expr->Token == EX_NameConst || Expr->Token == EX_StringConst || Expr->Token == EX_UnicodeStringConst)
		Value = TEXT("\"") + Value.ReplaceCharWithEscapedChar() + TEXT("\"");
	if (Expr->Token == EX_VectorConst) Value = FString::Printf(TEXT("(X=%.17g,Y=%.17g,Z=%.17g)"), Expr->VectorValue.X, Expr->VectorValue.Y, Expr->VectorValue.Z);
	if (Expr->Token == EX_RotationConst) Value = FString::Printf(TEXT("(Pitch=%.17g,Yaw=%.17g,Roll=%.17g)"), Expr->RotatorValue.Pitch, Expr->RotatorValue.Yaw, Expr->RotatorValue.Roll);
	return true;
}

void RestoreFormatTextNodes(UEdGraph* Graph)
{
	const auto Nodes = Graph->Nodes;
	for (UEdGraphNode* Node : Nodes)
	{
		auto Call = Cast<UK2Node_CallFunction>(Node);
		if (!Call || Call->GetTargetFunction() != UKismetTextLibrary::StaticClass()->FindFunctionByName(TEXT("Format"))) continue;
		auto Args = Call->FindPin(TEXT("InArgs"));
		auto Pattern = Call->FindPin(TEXT("InPattern"));
		auto Return = Call->FindPin(UEdGraphSchema_K2::PN_ReturnValue);
		if (!Args || !Pattern || !Return || Args->LinkedTo.Num() != 1) continue;
		auto Array = Cast<UK2Node_MakeArray>(Args->LinkedTo[0]->GetOwningNode());
		if (!Array) continue;
		TArray<UK2Node_MakeStruct*> Structs;
		TArray<TPair<FName, UEdGraphPin*>> Values;
		bool Valid = true;
		for (UEdGraphPin* Pin : Array->Pins)
		{
			if (Pin->Direction != EGPD_Input) continue;
			auto Struct = Pin->LinkedTo.Num() == 1 ? Cast<UK2Node_MakeStruct>(Pin->LinkedTo[0]->GetOwningNode()) : nullptr;
			if (!Struct || !Struct->StructType || Struct->StructType->GetFName() != TEXT("FormatArgumentData")) { Valid = false; break; }
			auto Name = Struct->FindPin(TEXT("ArgumentName"));
			auto Type = Struct->FindPin(TEXT("ArgumentValueType"));
			if (!Name || !Type || !UE427IsEmpty(Name->LinkedTo) || !UE427IsEmpty(Type->LinkedTo)) { Valid = false; break; }
			FString TypeName = Type->DefaultValue;
			if (const auto Enum = Cast<UEnum>(Type->PinType.PinSubCategoryObject.Get()))
			{
				const int64 Index = TypeName.IsNumeric() ? FCString::Atoi64(*TypeName) : Enum->GetValueByNameString(TypeName);
				TypeName = Enum->GetNameStringByValue(Index);
			}
			FName Field;
			if (TypeName == TEXT("Int") || TypeName == TEXT("0")) Field = TEXT("ArgumentValueInt");
			else if (TypeName == TEXT("Float") || TypeName == TEXT("2")) Field = TEXT("ArgumentValueFloat");
			else if (TypeName == TEXT("Double") || TypeName == TEXT("3")) Field = TEXT("ArgumentValueDouble");
			else if (TypeName == TEXT("Text") || TypeName == TEXT("4")) Field = TEXT("ArgumentValue");
			else if (TypeName == TEXT("Gender") || TypeName == TEXT("5")) Field = TEXT("ArgumentValueGender");
			auto Value = Struct->FindPin(Field);
			if (!Value || (UE427IsEmpty(Value->LinkedTo) && Field == TEXT("ArgumentValueGender"))) { Valid = false; break; }
			Structs.AddUnique(Struct);
			Values.Emplace(FName(*Name->DefaultValue), Value);
		}
		if (!Valid) continue;
		auto Format = NewObject<UK2Node_FormatText>(Graph);
		Format->CreateNewGuid();
		Format->NodePosX = Call->NodePosX;
		Format->NodePosY = Call->NodePosY;
		Graph->AddNode(Format, false, false);
		Format->AllocateDefaultPins();
		auto CopyInput = [](UEdGraphPin* Source, UEdGraphPin* Target)
		{
			Target->DefaultValue = Source->DefaultValue;
			Target->DefaultTextValue = Source->DefaultTextValue;
			Target->DefaultObject = Source->DefaultObject;
			for (UEdGraphPin* Link : Source->LinkedTo) Link->MakeLinkTo(Target);
		};
		CopyInput(Pattern, Format->GetFormatPin());
		Format->PinDefaultValueChanged(Format->GetFormatPin());
		for (const auto& Pair : Values)
		{
			auto Target = Format->FindArgumentPin(Pair.Key);
			if (!Target)
			{
				Format->AddArgumentPin();
				Format->SetArgumentName(Format->GetArgumentCount() - 1, Pair.Key);
				Target = Format->FindArgumentPin(Pair.Key);
			}
			if (!Target) continue;
			auto Source = Pair.Value;
			if (UE427IsEmpty(Source->LinkedTo))
			{
				FName LiteralName = TEXT("MakeLiteralText");
				if (Source->PinType.PinCategory == UEdGraphSchema_K2::PC_Int64) LiteralName = TEXT("MakeLiteralInt64");
				else if (Source->PinType.PinCategory == UEdGraphSchema_K2::PC_Float) LiteralName = Source->PinType.PinSubCategory == UEdGraphSchema_K2::PC_Float ? TEXT("MakeLiteralFloat") : TEXT("MakeLiteralDouble");
				auto Literal = NewObject<UK2Node_CallFunction>(Graph);
				Literal->CreateNewGuid();
				Literal->SetFromFunction(UKismetSystemLibrary::StaticClass()->FindFunctionByName(LiteralName));
				Graph->AddNode(Literal, false, false);
				Literal->AllocateDefaultPins();
				Literal->NodePosX = Format->NodePosX - 240;
				Literal->NodePosY = Format->NodePosY + 100;
				CopyInput(Source, Literal->FindPinChecked(TEXT("Value")));
				Literal->FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue)->MakeLinkTo(Target);
			}
			else CopyInput(Source, Target);
			Format->PinConnectionListChanged(Target);
		}
		for (UEdGraphPin* Link : Return->LinkedTo) Format->FindPinChecked(TEXT("Result"))->MakeLinkTo(Link);
		Call->BreakAllNodeLinks();
		Graph->RemoveNode(Call);
		auto RemoveUnused = [Graph](UEdGraphNode* Candidate)
		{
			for (UEdGraphPin* Pin : Candidate->Pins)
				if (Pin->Direction == EGPD_Output && !UE427IsEmpty(Pin->LinkedTo)) return;
			Candidate->BreakAllNodeLinks();
			Graph->RemoveNode(Candidate);
		};
		RemoveUnused(Array);
		for (auto Struct : Structs) RemoveUnused(Struct);
	}
}

FString String(const TSharedPtr<FJsonObject>& Object, const TCHAR* Field)
{
	FString Value;
	if (Object) Object->TryGetStringField(Field, Value);
	return Value;
}

struct FReader
{
	UClass* Owner;
	UFunction* Function = nullptr;
	TMap<FString, UFunction*> Signatures;
	TMap<FString, FString> AnimationLengthSources;
	FString Error;

	UObject* Object(const FString& Path)
	{
		if (UE427IsEmpty(Path) || Path == TEXT("None")) return nullptr;
		if (Path == Owner->GetName() || Path == Owner->GetPathName()) return Owner;
		UObject* Result = StaticFindObject(UObject::StaticClass(), nullptr, *Path);
		if (!Result) Result = LoadObject<UObject>(nullptr, *Path);
		if (!Result) {
			const FString Redirected = FCoreRedirects::GetRedirectedName(ECoreRedirectFlags::Type_Class, FCoreRedirectObjectName(Path)).ToString();
			if (Redirected != Path) Result = LoadObject<UClass>(nullptr, *Redirected, nullptr, LOAD_Quiet | LOAD_NoWarn);
		}
		if (!Result) Error = TEXT("Cannot resolve object ") + Path;
		return Result;
	}

	UStruct* Scope(const TSharedPtr<FExpr>& Expr)
	{
		if (!Expr) return Owner;
		if (Expr->Token == EX_InterfaceContext && !UE427IsEmpty(Expr->Children)) return Scope(Expr->Children[0]);
		if (Expr->Token == EX_Context && !UE427IsEmpty(Expr->Children)) return Scope(Expr->Children.Last());
		if (Expr->Token == EX_SwitchValue && Expr->Children.Num() >= 2) return Scope(Expr->Children.Last());
		if (UClass* Class = Cast<UClass>(Expr->ObjectRef)) return Class;
		if (Expr->ObjectRef) return Expr->ObjectRef->GetClass();
		if (Expr->ClassRef) return Expr->ClassRef;
		if (auto Property = CastField<FClassProperty>(Expr->PropertyRef)) return Property->MetaClass;
		if (auto Property = CastField<FObjectPropertyBase>(Expr->PropertyRef)) return Property->PropertyClass;
		if (auto Property = CastField<FInterfaceProperty>(Expr->PropertyRef)) return Property->InterfaceClass;
		if (auto Property = CastField<FStructProperty>(Expr->PropertyRef)) return Property->Struct;
		FProperty* ResultProperty = Expr->FunctionRef ? Expr->FunctionRef->GetReturnProperty() : nullptr;
		if (Expr->Token == EX_ArrayGetByRef && !UE427IsEmpty(Expr->Children))
			if (auto Array = CastField<FArrayProperty>(Expr->Children[0]->PropertyRef)) ResultProperty = Array->Inner;
		if (auto Property = CastField<FClassProperty>(ResultProperty)) return Property->MetaClass;
		if (auto Property = CastField<FObjectPropertyBase>(ResultProperty)) return Property->PropertyClass;
		if (auto Property = CastField<FStructProperty>(ResultProperty)) return Property->Struct;
		if (auto Property = CastField<FInterfaceProperty>(ResultProperty)) return Property->InterfaceClass;
		return Owner;
	}

	TSharedPtr<FExpr> Read(const TSharedPtr<FJsonObject>& Json, UStruct* Context = nullptr, int32 Depth = 0)
	{
		auto Expr = MakeShared<FExpr>();
		Expr->Token = EX_Nothing;
		if (!Json || Depth > 128) { this->Error = TEXT("Invalid or excessively nested Kismet expression"); return Expr; }
		if (!Context) Context = Owner;
		const FString Inst = String(Json, TEXT("Inst"));
		auto Child = [&](const TCHAR* Key, UStruct* ChildContext = nullptr) -> TSharedPtr<FExpr>
		{
			const TSharedPtr<FJsonObject>* Value = nullptr;
			if (!Json->TryGetObjectField(Key, Value)) { this->Error = TEXT("Missing expression ") + FString(Key) + TEXT(" in ") + Inst; return MakeShared<FExpr>(); }
			return Read(*Value, ChildContext ? ChildContext : Context, Depth + 1);
		};
		auto Number = [&](const TCHAR* Key) { double Value = 0; if (!Json->TryGetNumberField(Key, Value)) this->Error = TEXT("Missing numeric field ") + FString(Key) + TEXT(" in ") + Inst; return Value; };
		auto Arguments = [&](const TCHAR* Key)
		{
			const TArray<TSharedPtr<FJsonValue>>* Values = nullptr;
			if (!Json->TryGetArrayField(Key, Values)) { this->Error = TEXT("Missing array ") + FString(Key); return; }
			for (const auto& Value : *Values) Expr->Children.Add(Read(Value->AsObject(), Owner, Depth + 1));
		};
		auto Property = [&](const FString& Name, UStruct* PropertyScope)
		{
			FProperty* Result = FindFProperty<FProperty>(PropertyScope, *Name);
			if (!Result && PropertyScope == Function)
				for (const auto& Pair : Signatures)
					if (Pair.Key.StartsWith(TEXT("ExecuteUbergraph_"))) Result = FindFProperty<FProperty>(Pair.Value, *Name);
			if (!Result) this->Error = TEXT("Cannot resolve property ") + GetPathNameSafe(PropertyScope) + TEXT(".") + Name;
			return Result;
		};
		if (Inst == TEXT("Nothing") || Inst == TEXT("EndOfScript") || Inst == TEXT("Self") || Inst == TEXT("NoObject") || Inst == TEXT("NoInterface") || Inst == TEXT("PopExecutionFlow"))
		{
			if (Inst == TEXT("EndOfScript")) Expr->Token = EX_EndOfScript;
			if (Inst == TEXT("Self")) Expr->Token = EX_Self;
			if (Inst == TEXT("NoObject")) Expr->Token = EX_NoObject;
			if (Inst == TEXT("NoInterface")) Expr->Token = EX_NoInterface;
			if (Inst == TEXT("PopExecutionFlow")) Expr->Token = EX_PopExecutionFlow;
		}
		else if (Inst == TEXT("IntConst") || Inst == TEXT("IntConstByte") || Inst == TEXT("IntZero") || Inst == TEXT("IntOne") || Inst == TEXT("SkipOffsetConst"))
		{
			Expr->Token = EX_IntConst;
			Expr->IntValue = Inst == TEXT("IntZero") ? 0 : Inst == TEXT("IntOne") ? 1 : (int32)Number(TEXT("Value"));
		}
		else if (Inst == TEXT("FloatConst") || Inst == TEXT("DoubleConst"))
		{
			Expr->Token = Inst == TEXT("DoubleConst") ? EX_DoubleConst : EX_FloatConst;
			Expr->FloatValue = Number(TEXT("Value"));
		}
		else if (Inst == TEXT("ByteConst")) { Expr->Token = EX_ByteConst; Expr->ByteValue = Number(TEXT("Value")); }
		else if (Inst == TEXT("Int64Const") || Inst == TEXT("UInt64Const"))
		{
			Expr->Token = EX_Int64Const;
			FString Value = String(Json, TEXT("Value"));
			Expr->Int64Value = UE427IsEmpty(Value) ? (int64)Number(TEXT("Value")) : FCString::Atoi64(*Value);
		}
		else if (Inst == TEXT("True") || Inst == TEXT("False")) { Expr->Token = Inst == TEXT("True") ? EX_True : EX_False; Expr->BoolValue = Inst == TEXT("True"); }
		else if (Inst == TEXT("BitFieldConst")) { Expr->BoolValue = Number(TEXT("Value")) != 0; Expr->Token = Expr->BoolValue ? EX_True : EX_False; }
		else if (Inst == TEXT("StringConst") || Inst == TEXT("UnicodeStringConst")) { Expr->Token = EX_StringConst; Expr->StringValue = String(Json, TEXT("Value")); }
		else if (Inst == TEXT("NameConst")) { Expr->Token = EX_NameConst; Expr->NameValue = *String(Json, TEXT("Value")); }
		else if (Inst == TEXT("TextConst"))
		{
			Expr->Token = EX_TextConst;
			const FString Kind = String(Json, TEXT("TextLiteralType"));
			Expr->StringValue = String(Json, TEXT("SourceString"));
			Expr->NameValue = *String(Json, TEXT("LocalizationKey"));
			if (Kind == TEXT("LocalizedText"))
			{
				Expr->ByteValue = 1;
				auto Namespace = MakeShared<FExpr>();
				Namespace->Token = EX_StringConst;
				Namespace->StringValue = String(Json, TEXT("LocalizationNamespace"));
				Expr->Children.Add(Namespace);
			}
			else if (Kind == TEXT("StringTableEntry")) {
				Expr->Token = EX_CallMath;
				Expr->FunctionRef = UKismetTextLibrary::StaticClass()->FindFunctionByName(TEXT("TextFromStringTable"));
				auto Table = MakeShared<FExpr>();
				Table->Token = EX_NameConst;
				Table->NameValue = *String(Json, TEXT("TableId"));
				auto Key = MakeShared<FExpr>();
				Key->Token = EX_StringConst;
				Key->StringValue = String(Json, TEXT("TableKey"));
				Expr->Children = {Table, Key};
			}
		}
		else if (Inst == TEXT("ObjectConst")) { Expr->Token = EX_ObjectConst; Expr->ObjectRef = Object(String(Json, TEXT("Object"))); }
		else if (Inst == TEXT("VectorConst")) { Expr->Token = EX_VectorConst; Expr->VectorValue = FVector(Number(TEXT("X")), Number(TEXT("Y")), Number(TEXT("Z"))); }
		else if (Inst == TEXT("RotationConst")) { Expr->Token = EX_RotationConst; Expr->RotatorValue = FRotator(Number(TEXT("Pitch")), Number(TEXT("Yaw")), Number(TEXT("Roll"))); }
		else if (Inst == TEXT("TransformConst"))
		{
			Expr->Token = EX_CallMath;
			Expr->FunctionRef = UKismetMathLibrary::StaticClass()->FindFunctionByName(TEXT("MakeTransform"));
			const auto Translation = Json->GetObjectField(TEXT("Translation"));
			const auto Rotation = Json->GetObjectField(TEXT("Rotation"));
			const auto Scale = Json->GetObjectField(TEXT("Scale"));
			auto PositionExpr = MakeShared<FExpr>();
			PositionExpr->Token = EX_VectorConst;
			PositionExpr->VectorValue = FVector(Translation->GetNumberField(TEXT("X")), Translation->GetNumberField(TEXT("Y")), Translation->GetNumberField(TEXT("Z")));
			auto RotationExpr = MakeShared<FExpr>();
			RotationExpr->Token = EX_RotationConst;
			RotationExpr->RotatorValue = FQuat(Rotation->GetNumberField(TEXT("X")), Rotation->GetNumberField(TEXT("Y")), Rotation->GetNumberField(TEXT("Z")), Rotation->GetNumberField(TEXT("W"))).Rotator();
			auto ScaleExpr = MakeShared<FExpr>();
			ScaleExpr->Token = EX_VectorConst;
			ScaleExpr->VectorValue = FVector(Scale->GetNumberField(TEXT("X")), Scale->GetNumberField(TEXT("Y")), Scale->GetNumberField(TEXT("Z")));
			Expr->Children = {PositionExpr, RotationExpr, ScaleExpr};
		}
		else if (Inst == TEXT("LocalVariable") || Inst == TEXT("LocalOutVariable") || Inst == TEXT("InstanceVariable") || Inst == TEXT("DefaultVariable"))
		{
			const bool Local = Inst == TEXT("LocalVariable") || Inst == TEXT("LocalOutVariable");
			if (!Local && Context == Owner)
				if (const FString* Source = AnimationLengthSources.Find(String(Json, TEXT("VariableName"))))
				{
					auto Variable = MakeShared<FExpr>();
					Variable->Token = EX_InstanceVariable;
					Variable->PropertyRef = Property(*Source, Owner);
					const auto ObjectProperty = CastField<FObjectPropertyBase>(Variable->PropertyRef);
					auto Call = MakeShared<FExpr>();
					Call->Token = EX_FinalFunction;
					Call->NameValue = TEXT("GetPlayLength");
					Call->FunctionRef = ObjectProperty ? ObjectProperty->PropertyClass->FindFunctionByName(Call->NameValue) : nullptr;
					if (!Call->FunctionRef) this->Error = TEXT("Cannot recover animation length binding for ") + *Source;
					Expr->Token = EX_Context;
					Expr->ContextObject = Variable;
					Expr->Children.Add(Call);
					return Expr;
				}
			Expr->Token = Local ? EX_LocalVariable : EX_InstanceVariable;
			if (Inst == TEXT("LocalOutVariable")) Expr->Token = EX_LocalOutVariable;
			Expr->PropertyRef = Property(String(Json, TEXT("VariableName")), Local ? (UStruct*)Function : Context);
			const TSharedPtr<FJsonObject>* Type = nullptr;
			if (Json->TryGetObjectField(TEXT("VariableType"), Type))
			{
				const FString ClassPath = String(*Type, TEXT("PinSubCategoryObject"));
				if (!UE427IsEmpty(ClassPath) && CastField<FObjectPropertyBase>(Expr->PropertyRef)) Expr->ClassRef = Cast<UClass>(Object(ClassPath));
			}
		}
		else if (Inst == TEXT("Context") || Inst == TEXT("Context_FailSilent") || Inst == TEXT("ClassContext"))
		{
			Expr->Token = EX_Context;
			Expr->ContextObject = Child(TEXT("Context"));
			Expr->Children.Add(Child(TEXT("Expression"), Scope(Expr->ContextObject)));
			if (Inst == TEXT("ClassContext"))
			{
				if (auto Class = Cast<UClass>(Expr->ContextObject->ObjectRef)) Expr->ContextObject->ObjectRef = Class->GetDefaultObject();
				else Expr->Token = EX_ClassContext;
			}
		}
		else if (Inst == TEXT("FinalFunction") || Inst == TEXT("LocalFinalFunction") || Inst == TEXT("VirtualFunction") || Inst == TEXT("LocalVirtualFunction") || Inst == TEXT("CallMath"))
		{
			FString Name = String(Json, TEXT("Function"));
			if (UE427IsEmpty(Name)) Name = String(Json, TEXT("FunctionName"));
			UClass* Class = Cast<UClass>(Context);
			const FString FunctionOwner = String(Json, TEXT("FunctionOwner"));
			if (!UE427IsEmpty(FunctionOwner)) Class = Cast<UClass>(Object(FunctionOwner));
			if (Inst == TEXT("CallMath")) Class = Cast<UClass>(Object(String(Json, TEXT("ContextClass"))));
			Expr->Token = Inst == TEXT("CallMath") ? EX_CallMath : Inst == TEXT("VirtualFunction") ? EX_VirtualFunction : Inst == TEXT("LocalVirtualFunction") ? EX_LocalVirtualFunction : Inst == TEXT("LocalFinalFunction") ? EX_LocalFinalFunction : EX_FinalFunction;
			Expr->NameValue = *Name;
			Expr->FunctionRef = Context == Owner && FunctionOwner.IsEmpty() && Inst != TEXT("CallMath") ? Signatures.FindRef(Name) : nullptr;
			// Final calls to overridden events bypass virtual dispatch and invoke the parent.
			if (Context == Owner && FunctionOwner.IsEmpty() && Class && Class->GetSuperClass() &&
				(Inst == TEXT("FinalFunction") || Inst == TEXT("LocalFinalFunction")))
				if (auto ParentFunction = Class->GetSuperClass()->FindFunctionByName(*Name))
					if (ParentFunction->HasAnyFunctionFlags(FUNC_BlueprintEvent)) Expr->FunctionRef = ParentFunction;
            if (!Expr->FunctionRef) Expr->FunctionRef = Class ? Class->FindFunctionByName(*Name) : nullptr;
			if (!Expr->FunctionRef) this->Error = TEXT("Cannot resolve function ") + GetNameSafe(Class) + TEXT(".") + Name;
			Arguments(TEXT("Parameters"));
			if (GetPathNameSafe(Class) == TEXT("/Script/Engine.BlueprintTypeConversions") && Expr->Children.Num() == 1
				&& (Name == TEXT("ConvertFVector2dToFVector2f") || Name == TEXT("ConvertFVector2fToFVector2d") || Name == TEXT("ConvertMapType") || Name == TEXT("ConvertArrayType") || Name == TEXT("ConvertSetType"))) return Expr->Children[0];
		}
		else if (Inst == TEXT("Let") || Inst == TEXT("LetObj") || Inst == TEXT("LetBool") || Inst == TEXT("LetWeakObjPtr") || Inst == TEXT("LetDelegate") || Inst == TEXT("LetMulticastDelegate"))
		{
			Expr->Token = EX_Let;
			Expr->Children.Add(Child(TEXT("Variable")));
			Expr->Children.Add(Child(TEXT("Expression")));
			Expr->PropertyRef = Expr->Children[0]->PropertyRef;
		}
		else if (Inst == TEXT("LetValueOnPersistentFrame"))
		{
			Expr->Token = EX_LetValueOnPersistentFrame;
			Expr->PropertyRef = Property(String(Json, TEXT("PropertyName")), Function);
			Expr->Children.Add(Child(TEXT("Expression")));
		}
		else if (Inst == TEXT("Jump") || Inst == TEXT("JumpIfNot") || Inst == TEXT("PushExecutionFlow"))
		{
			Expr->Token = Inst == TEXT("Jump") ? EX_Jump : Inst == TEXT("JumpIfNot") ? EX_JumpIfNot : EX_PushExecutionFlow;
			Expr->JumpTarget = Number(TEXT("Offset"));
			if (Inst == TEXT("JumpIfNot")) Expr->Children.Add(Child(TEXT("Condition")));
		}
		else if (Inst == TEXT("Return") || Inst == TEXT("PopExecutionFlowIfNot") || Inst == TEXT("ComputedJump") || Inst == TEXT("InterfaceContext") || Inst == TEXT("SoftObjectConst"))
		{
			Expr->Token = Inst == TEXT("Return") ? EX_Return : Inst == TEXT("ComputedJump") ? EX_ComputedJump : Inst == TEXT("InterfaceContext") ? EX_InterfaceContext : Inst == TEXT("SoftObjectConst") ? EX_SoftObjectConst : EX_PopExecutionFlowIfNot;
			Expr->Children.Add(Child(Inst == TEXT("ComputedJump") ? TEXT("OffsetExpression") : Inst == TEXT("PopExecutionFlowIfNot") ? TEXT("Condition") : Inst == TEXT("SoftObjectConst") ? TEXT("Value") : TEXT("Expression")));
		}
		else if (Inst == TEXT("DynamicCast") || Inst == TEXT("MetaCast") || Inst == TEXT("ObjToInterfaceCast") || Inst == TEXT("CrossInterfaceCast") || Inst == TEXT("InterfaceToObjCast"))
		{
			Expr->Token = Inst == TEXT("DynamicCast") ? EX_DynamicCast : Inst == TEXT("MetaCast") ? EX_MetaCast : Inst == TEXT("ObjToInterfaceCast") ? EX_ObjToInterfaceCast : Inst == TEXT("CrossInterfaceCast") ? EX_CrossInterfaceCast : EX_InterfaceToObjCast;
			Expr->ClassRef = Cast<UClass>(Object(String(Json, Inst == TEXT("InterfaceToObjCast") ? TEXT("ObjectClass") : Inst.Contains(TEXT("Interface")) ? TEXT("InterfaceClass") : TEXT("Class"))));
			Expr->Children.Add(Child(TEXT("Expression")));
		}
		else if (Inst == TEXT("ArrayGetByRef")) { Expr->Token = EX_ArrayGetByRef; Expr->Children.Add(Child(TEXT("ArrayExpression"))); Expr->Children.Add(Child(TEXT("IndexExpression"))); }
		else if (Inst == TEXT("BindDelegate"))
		{
			Expr->Token = EX_BindDelegate;
			Expr->NameValue = *String(Json, TEXT("FunctionName"));
			Expr->Children.Add(Child(TEXT("Delegate")));
			Expr->Children.Add(Child(TEXT("Object")));
		}
		else if (Inst == TEXT("AddMulticastDelegate") || Inst == TEXT("RemoveMulticastDelegate") || Inst == TEXT("ClearMulticastDelegate"))
		{
			Expr->Token = Inst == TEXT("AddMulticastDelegate") ? EX_AddMulticastDelegate : Inst == TEXT("RemoveMulticastDelegate") ? EX_RemoveMulticastDelegate : EX_ClearMulticastDelegate;
			Expr->Children.Add(Child(TEXT("MulticastDelegate")));
			if (Inst != TEXT("ClearMulticastDelegate")) Expr->Children.Add(Child(TEXT("Delegate")));
		}
		else if (Inst == TEXT("CallMulticastDelegate"))
		{
			Expr->Token = EX_CallMulticastDelegate;
			Expr->Children.Add(Child(TEXT("Delegate")));
			auto DelegateExpression = Expr->Children[0];
			while ((DelegateExpression->Token == EX_Context || DelegateExpression->Token == EX_Context_FailSilent || DelegateExpression->Token == EX_ClassContext) && DelegateExpression->Children.Num() == 1) DelegateExpression = DelegateExpression->Children[0];
			if (auto Delegate = CastField<FMulticastDelegateProperty>(DelegateExpression->PropertyRef)) Expr->FunctionRef = Delegate->SignatureFunction;
			if (!Expr->FunctionRef) this->Error = TEXT("Cannot resolve multicast delegate signature");
			Arguments(TEXT("Parameters"));
		}
		else if (Inst == TEXT("MapConst") || Inst == TEXT("SetConst"))
		{
			Expr->Token = Inst == TEXT("MapConst") ? EX_MapConst : EX_SetConst;
			Arguments(TEXT("Values"));
			if (!UE427IsEmpty(Expr->Children)) this->Error = TEXT("Nonempty container constant requires key and value type metadata");
		}
		else if (Inst == TEXT("SetMap"))
		{
			Expr->Token = EX_SetMap;
			Expr->Children.Add(Child(TEXT("LeftSideExpression")));
			const TArray<TSharedPtr<FJsonValue>>* Pairs = nullptr;
			if (!Json->TryGetArrayField(TEXT("Values"), Pairs)) this->Error = TEXT("Missing map entries");
			else for (const auto& Pair : *Pairs)
			{
				const TSharedPtr<FJsonObject>* Key = nullptr;
				const TSharedPtr<FJsonObject>* Value = nullptr;
				if (!Pair->AsObject()->TryGetObjectField(TEXT("Key"), Key) || !Pair->AsObject()->TryGetObjectField(TEXT("Value"), Value)) { this->Error = TEXT("Invalid map entry"); break; }
				Expr->Children.Add(Read(*Key, Context, Depth + 1));
				Expr->Children.Add(Read(*Value, Context, Depth + 1));
			}
		}
		else if (Inst == TEXT("SetArray") || Inst == TEXT("ArrayConst"))
		{
			Expr->Token = Inst == TEXT("SetArray") ? EX_SetArray : EX_ArrayConst;
			if (Inst == TEXT("SetArray")) Expr->Children.Add(Child(TEXT("LeftSideExpression")));
			Arguments(TEXT("Values"));
			Expr->IntValue = Expr->Children.Num() - (Inst == TEXT("SetArray") ? 1 : 0);
		}
		else if (Inst == TEXT("PrimitiveCast"))
		{
			const FString Kind = String(Json, TEXT("CastType"));
			Expr->Token = EX_CallMath;
			Expr->Children.Add(Child(TEXT("Expression")));
			if (Kind == TEXT("ObjectToBool") || Kind == TEXT("InterfaceToBool"))
			{
				if (Kind == TEXT("InterfaceToBool"))
				{
					auto Conversion = MakeShared<FExpr>();
					Conversion->Token = EX_CallMath;
					Conversion->FunctionRef = UKismetSystemLibrary::StaticClass()->FindFunctionByName(TEXT("Conv_InterfaceToObject"));
					Conversion->Children = Expr->Children;
					Expr->Children = {Conversion};
				}
				Expr->FunctionRef = UKismetMathLibrary::StaticClass()->FindFunctionByName(TEXT("NotEqual_ObjectObject"));
				auto Null = MakeShared<FExpr>();
				Null->Token = EX_NoObject;
				Expr->Children.Add(Null);
			}
			else if (Kind == TEXT("FloatToDouble") || Kind == TEXT("DoubleToFloat")) Expr->FunctionRef = UKismetMathLibrary::StaticClass()->FindFunctionByName(*(TEXT("Conv_") + Kind));
			else this->Error = TEXT("Unsupported primitive conversion ") + Kind;
		}
		else if (Inst == TEXT("SwitchValue"))
		{
			Expr->Token = EX_SwitchValue;
			Expr->Children.Add(Child(TEXT("Expression")));
			const TArray<TSharedPtr<FJsonValue>>* Cases = nullptr;
			if (!Json->TryGetArrayField(TEXT("Cases"), Cases)) this->Error = TEXT("Missing switch cases");
			else
			{
				Expr->IntValue = Cases->Num();
				for (const auto& Case : *Cases)
				{
					const auto CaseJson = Case->AsObject();
					Expr->Children.Add(Read(CaseJson->GetObjectField(TEXT("CaseValue")), Context, Depth + 1));
					Expr->Children.Add(Read(CaseJson->GetObjectField(TEXT("CaseResult")), Context, Depth + 1));
				}
			}
			Expr->Children.Add(Child(TEXT("DefaultResult")));
		}
		else if (Inst == TEXT("StructMemberContext"))
		{
			Expr->Token = EX_StructMemberContext;
			Expr->Children.Add(Child(TEXT("StructExpression")));
			Expr->PropertyRef = Property(String(Json, TEXT("PropertyName")), Scope(Expr->Children[0]));
		}
		else if (Inst == TEXT("StructConst"))
		{
			Expr->Token = EX_StructConst;
			Expr->StructRef = Cast<UScriptStruct>(Object(String(Json, TEXT("Struct"))));
			const TSharedPtr<FJsonObject>* Properties = nullptr;
			if (!Json->TryGetObjectField(TEXT("Properties"), Properties)) this->Error = TEXT("Missing struct constant properties");
			else for (const auto& Pair : (*Properties)->Values)
				for (const auto& Value : Pair.Value->AsArray()) Expr->Children.Add(Read(Value->AsObject(), Context, Depth + 1));
			RenderConstant(Expr, Expr->StringValue);
		}
		else if (Inst == TEXT("Breakpoint") || Inst == TEXT("Tracepoint") || Inst == TEXT("WireTracepoint") || Inst == TEXT("InstrumentationEvent")) Expr->Token = EX_Nothing;
		else this->Error = TEXT("Unsupported Kismet instruction ") + Inst;
		return Expr;
	}
};
}

bool FCASJsonGraph::Generate(UBlueprint* Blueprint, const TSharedPtr<FJsonObject>& AssetData, UObjectHierarchySerializer* Serializer, FString& OutError)
{
	OutError.Reset();
	FString PlaceholderReason;
	if (AssetData->TryGetStringField(TEXT("PlaceholderGraphReason"), PlaceholderReason) && !UE427IsEmpty(PlaceholderReason)) return false;
	if (!Blueprint || !Blueprint->GeneratedClass || Blueprint->BlueprintType == BPTYPE_Interface) return false;
	const TArray<TSharedPtr<FJsonValue>>* Children = nullptr;
	if (!AssetData->TryGetArrayField(TEXT("Children"), Children)) return false;
	const TSharedPtr<FJsonObject>* TimelineData = nullptr;
	const TArray<TSharedPtr<FJsonValue>>* TimelineIndices = nullptr;
	bool bRestoredTimelines = false;
	if (AssetData->TryGetObjectField(TEXT("AssetObjectData"), TimelineData) && (*TimelineData)->TryGetArrayField(TEXT("Timelines"), TimelineIndices))
		for (const auto& Index : *TimelineIndices)
			if (auto Template = Cast<UTimelineTemplate>(Serializer->DeserializeObject((int32)Index->AsNumber())))
			{
				if (Blueprint->Timelines.ContainsByPredicate([Template](const UTimelineTemplate* Existing) { return Existing && Existing->GetVariableName() == Template->GetVariableName(); })) continue;
				Blueprint->Timelines.Add(Template);
				bRestoredTimelines = true;
			}
	if (bRestoredTimelines)
	{
		FBlueprintCompilationManager::CompileSynchronously(FBPCompileRequest(Blueprint, EBlueprintCompileOptions::SkipDefaultObjectValidation, nullptr));
		Serializer->SetObjectMark(Blueprint->GeneratedClass, TEXT("$AssetObject$"));
	}
	FString Source;
	FJsonSerializer::Serialize(AssetData.ToSharedRef(), TJsonWriterFactory<>::Create(&Source));
	const FString SourceHash = FMD5::HashAnsiString(*(TEXT("CASGraphs-20:") + Source));
	auto&& MetadataStorage = Blueprint->GetOutermost()->GetMetaData();
	auto Metadata = [](auto&& Value) { if constexpr (std::is_pointer_v<std::decay_t<decltype(Value)>>) return Value; else return &Value; }(MetadataStorage);
	if (!bRestoredTimelines && Blueprint->Status != BS_Error && Metadata->GetValue(Blueprint, TEXT("CASGraphSource")) == SourceHash) return false;
	TStrongObjectPtr<UClass> SignatureOwner(NewObject<UClass>(GetTransientPackage()));
	SignatureOwner->SetSuperStruct(UObject::StaticClass());
	SignatureOwner->ClassWithin = UObject::StaticClass();
	SignatureOwner->Bind();
	SignatureOwner->StaticLink(true);
	FReader Reader{ Blueprint->GeneratedClass };
	const TSharedPtr<FJsonObject>* AnimationData = nullptr;
	TSet<FString> RecoveredEvaluatorFunctions;
	TMap<FString, TSet<int32>> RecoveredEvaluatorOffsets;
	if (AssetData->TryGetObjectField(TEXT("AnimationBlueprintData"), AnimationData) &&
		(*AnimationData)->HasField(TEXT("InputRecoveryVersion")) &&
		UE427IsEmpty((*AnimationData)->GetArrayField(TEXT("InputRecoveryErrors"))))
	{
		TMap<FString, TSharedPtr<FJsonObject>> FunctionData;
		for (const auto& Child : *Children) FunctionData.Add(String(Child->AsObject(), TEXT("ObjectName")), Child->AsObject());
		const TArray<TSharedPtr<FJsonValue>>* AnimationFunctions = nullptr;
		if (AssetData->TryGetArrayField(TEXT("AnimationFunctions"), AnimationFunctions))
			for (const auto& Function : *AnimationFunctions) FunctionData.Add(String(Function->AsObject(), TEXT("ObjectName")), Function->AsObject());
		for (const auto& Node : (*AnimationData)->GetArrayField(TEXT("Nodes")))
		{
			const TSharedPtr<FJsonObject>* Handler = nullptr;
			if (!Node->AsObject()->TryGetObjectField(TEXT("ExposedValueHandler"), Handler)) continue;
			const FString Bound = String(*Handler, TEXT("BoundFunction"));
			const auto FunctionJson = FunctionData.FindRef(Bound);
			if (!FunctionJson) continue;
			const auto& EvaluatorScript = FunctionJson->GetArrayField(TEXT("Script"));
			if (UE427IsEmpty(EvaluatorScript)) continue;
			const auto Entry = EvaluatorScript[0]->AsObject();
			if (String(Entry, TEXT("Inst")) != TEXT("LocalFinalFunction")) continue;
			const FString Ubergraph = String(Entry, TEXT("Function"));
			const auto UbergraphJson = FunctionData.FindRef(Ubergraph);
			const auto& Arguments = Entry->GetArrayField(TEXT("Parameters"));
			if (!UbergraphJson || Arguments.Num() != 1 || String(Arguments[0]->AsObject(), TEXT("Inst")) != TEXT("IntConst")) continue;
			const auto& Script = UbergraphJson->GetArrayField(TEXT("Script"));
			int32 Offset = Arguments[0]->AsObject()->GetIntegerField(TEXT("Value"));
			TSet<int32> Offsets;
			TSet<int32> VisitedOffsets;
			bool bTerminated = false;
			while (!VisitedOffsets.Contains(Offset))
			{
				VisitedOffsets.Add(Offset);
				const int32 Index = Script.IndexOfByPredicate([&](const auto& Value) { return Value->AsObject()->GetIntegerField(TEXT("StatementIndex")) == Offset; });
				if (Index == INDEX_NONE) break;
				const auto Statement = Script[Index]->AsObject();
				const FString Inst = String(Statement, TEXT("Inst"));
				if (Inst == TEXT("PopExecutionFlow") || Inst == TEXT("Return") || Inst == TEXT("EndOfScript")) { bTerminated = true; break; }
				if (Inst == TEXT("Jump")) Offset = Statement->GetIntegerField(TEXT("Offset"));
				else
				{
					Offsets.Add(Offset);
					if (!Script.IsValidIndex(Index + 1)) break;
					Offset = Script[Index + 1]->AsObject()->GetIntegerField(TEXT("StatementIndex"));
				}
			}
			if (bTerminated)
			{
				RecoveredEvaluatorFunctions.Add(Bound);
				for (const int32 RecoveredOffset : Offsets) RecoveredEvaluatorOffsets.FindOrAdd(Ubergraph).Add(RecoveredOffset);
			}
		}
	}
	if (AssetData->TryGetObjectField(TEXT("AnimationBlueprintData"), AnimationData))
		for (const auto& Copy : (*AnimationData)->GetArrayField(TEXT("PropertyCopies")))
		{
			const auto& SourcePath = Copy->AsObject()->GetArrayField(TEXT("Source"));
			const auto& DestinationPath = Copy->AsObject()->GetArrayField(TEXT("Destination"));
			if (SourcePath.Num() == 2 && SourcePath[1]->AsString() == TEXT("SequenceLength") && DestinationPath.Num() == 1)
				Reader.AnimationLengthSources.Add(DestinationPath[0]->AsString(), SourcePath[0]->AsString());
		}
	FCompilerResultsLog Results;
	for (const auto& Child : *Children)
	{
		const auto Json = Child->AsObject();
		const FString Name = String(Json, TEXT("ObjectName"));
		if (!Json || UE427IsEmpty(Name)) continue;
		const EFunctionFlags FunctionFlags = (EFunctionFlags)FCString::Atoi64(*String(Json, TEXT("FunctionFlags")));
		if ((FunctionFlags & FUNC_Delegate) != 0) continue;
		UFunction* Function = NewObject<UFunction>(SignatureOwner.Get(), *Name);
		Function->FunctionFlags = FunctionFlags;
		const TArray<TSharedPtr<FJsonValue>>* Properties = nullptr;
		if (Json->TryGetArrayField(TEXT("ChildProperties"), Properties))
			for (int32 Index = Properties->Num() - 1; Index >= 0; --Index)
			{
				auto PropertyJson = (*Properties)[Index]->AsObject();
				FString SignatureName;
				if (PropertyJson && PropertyJson->TryGetStringField(TEXT("SignatureFunction"), SignatureName) && !SignatureName.StartsWith(TEXT("/")))
				{
					double SignatureIndex;
					UFunction* Signature = PropertyJson->TryGetNumberField(TEXT("SignatureFunctionObject"), SignatureIndex) && SignatureIndex >= 0 ? Cast<UFunction>(Serializer->DeserializeObject((int32)SignatureIndex)) : nullptr;
					if (!Signature) Signature = Blueprint->GeneratedClass->FindFunctionByName(FName(*SignatureName));
					if (!Signature) {
						for (TObjectIterator<UFunction> It; It; ++It) {
							UClass* OwnerClass = It->GetOwnerClass();
							if (It->GetFName() != FName(*SignatureName) || !It->HasAnyFunctionFlags(FUNC_Delegate)) continue;
							if (OwnerClass && (OwnerClass->HasAnyClassFlags(CLASS_NewerVersionExists) || OwnerClass->GetName().StartsWith(TEXT("SKEL_")))) continue;
							if (!OwnerClass && !It->GetOutermost()->GetName().StartsWith(TEXT("/Script/"))) continue;
							if (Signature) { Signature = nullptr; break; }
							Signature = *It;
						}
					}
					if (Signature)
					{
						PropertyJson = MakeShared<FJsonObject>(*PropertyJson);
						PropertyJson->SetStringField(TEXT("SignatureFunction"), Signature->GetPathName());
					}
				}
				FDeserializedProperty Property(PropertyJson, Serializer, true);
				if (Property.GraphPinType.PinCategory == NAME_None)
				{
					UE_LOG(LogCASGraph, Warning, TEXT("Cannot reconstruct %s.%s: unresolved type for %s"), *Blueprint->GetPathName(), *Name, *Property.PropertyName.ToString());
					OutError = TEXT("Graph reconstruction encountered an unresolved property type");
					return false;
				}
				if (FProperty* Field = FKismetCompilerUtilities::CreatePropertyOnScope(Function, Property.PropertyName, Property.GraphPinType, Blueprint->GeneratedClass, Property.PropertyFlags, GetDefault<UEdGraphSchema_K2>(), Results))
				{
					Field->ArrayDim = Property.ArrayDim;
					Function->AddCppProperty(Field);
				}
			}
		Function->StaticLink(true);
		Reader.Signatures.Add(Name, Function);
		// UE4 member references enumerate UFields rather than the function lookup map.
		Function->Next = SignatureOwner->Children;
		SignatureOwner->Children = Function;
		SignatureOwner->AddFunctionToFunctionMap(Function, *Name);
	}
	if (Results.NumErrors > 0) { OutError = TEXT("Cannot reconstruct function signatures"); UE_LOG(LogCASGraph, Warning, TEXT("Cannot reconstruct signatures for %s"), *Blueprint->GetPathName()); return false; }
	TArray<FCASDecompiledFunction> Functions;
	bool UbergraphFailed = false;
	bool SkippedFunction = false;
	for (const auto& Child : *Children)
	{
		const auto Json = Child->AsObject();
		const FString Name = String(Json, TEXT("ObjectName"));
		Reader.Function = Reader.Signatures.FindRef(Name);
		if (RecoveredEvaluatorFunctions.Contains(Name)) continue;
		const TArray<TSharedPtr<FJsonValue>>* Script = nullptr;
		if (!Reader.Function || !Json->TryGetArrayField(TEXT("Script"), Script) || UE427IsEmpty(*Script)) continue;
		Reader.Error.Reset();
		FCASDecompiledFunction Function;
		Function.OriginalFunction = Reader.Function;
		Function.FunctionName = Name;
		Function.bIsUbergraph = Name.StartsWith(TEXT("ExecuteUbergraph_"));
		for (const auto& Statement : *Script)
		{
			const auto* EvaluatorOffsets = RecoveredEvaluatorOffsets.Find(Name);
			TSharedPtr<FExpr> Expr;
			if (EvaluatorOffsets && EvaluatorOffsets->Contains(Statement->AsObject()->GetIntegerField(TEXT("StatementIndex"))))
			{
				Expr = MakeShared<FExpr>();
				Expr->Token = EX_Nothing;
			}
			else Expr = Reader.Read(Statement->AsObject());
			double Offset;
			if (!Statement->AsObject()->TryGetNumberField(TEXT("StatementIndex"), Offset) || Offset < 0 || Offset > MAX_int32 || Function.OffsetToStatementIndex.Contains((int32)Offset)) Reader.Error = TEXT("Invalid or duplicate bytecode offset");
			else Expr->StartOffset = (int32)Offset;
			Function.OffsetToStatementIndex.Add(Expr->StartOffset, Function.Statements.Num());
			Function.Statements.Add(Expr);
			if (!UE427IsEmpty(Reader.Error)) break;
		}
		for (int32 Index = 0; UE427IsEmpty(Reader.Error) && Index < Function.Statements.Num(); ++Index)
		{
			auto Expr = Function.Statements[Index];
			Expr->EndOffset = Index + 1 < Function.Statements.Num() ? Function.Statements[Index + 1]->StartOffset : Expr->StartOffset + 1;
			if ((Expr->Token == EX_Jump || Expr->Token == EX_JumpIfNot || Expr->Token == EX_PushExecutionFlow) && !Function.OffsetToStatementIndex.Contains(Expr->JumpTarget)) Reader.Error = TEXT("Jump target does not match a statement boundary");
			if (!Function.bIsUbergraph && Expr->IsFunctionCall() && Expr->NameValue.ToString().StartsWith(TEXT("ExecuteUbergraph_")) && Expr->Children.Num() == 1 && Expr->Children[0]->Token == EX_IntConst)
			{
				Function.bIsEventStub = true;
				Function.UbergraphEntryOffset = Expr->Children[0]->IntValue;
			}
		}
		if (!UE427IsEmpty(Reader.Error))
		{
			UE_LOG(LogCASGraph, Warning, TEXT("Graph reconstruction skipped %s.%s: %s"), *Blueprint->GetName(), *Name, *Reader.Error);
			UbergraphFailed |= Function.bIsUbergraph;
			SkippedFunction = true;
			OutError = Name + TEXT(": ") + Reader.Error;
			continue;
		}
		Functions.Add(MoveTemp(Function));
	}
	if (UbergraphFailed) Functions.RemoveAll([](const FCASDecompiledFunction& Function) { return Function.bIsEventStub; });
	for (auto& Function : Functions)
		if (Function.bIsUbergraph)
			for (const auto& Stub : Functions)
				if (Stub.bIsEventStub && Function.OffsetToStatementIndex.Contains(Stub.UbergraphEntryOffset)) Function.EventEntryPoints.Add(*Stub.FunctionName, Stub.UbergraphEntryOffset);
	if (UE427IsEmpty(Functions)) return false;
	FCASGraphBuilder Builder;
	const TSharedPtr<FJsonObject>* ObjectData = nullptr;
	const TArray<TSharedPtr<FJsonValue>>* Bindings = nullptr;
	if (AssetData->TryGetObjectField(TEXT("AssetObjectData"), ObjectData) && (*ObjectData)->TryGetArrayField(TEXT("DynamicBindingObjects"), Bindings))
		for (const auto& BindingValue : *Bindings)
			if (UComponentDelegateBinding* Binding = Cast<UComponentDelegateBinding>(Serializer->DeserializeObject((int32)BindingValue->AsNumber())))
				for (const FBlueprintComponentDelegateBinding& Entry : Binding->ComponentDelegateBindings)
					Builder.AddComponentDelegateBinding(Entry.FunctionNameToBind, Entry.ComponentPropertyName, Entry.DelegatePropertyName);
	Builder.BuildGraphs(Blueprint, Blueprint->GeneratedClass, Functions);
	if (!UE427IsEmpty(Builder.GetWarnings())) OutError = TEXT("Graph reconstruction reported builder warnings; see LogGraphBuilder");
	TArray<UEdGraph*> Graphs;
	Blueprint->GetAllGraphs(Graphs);
	for (UEdGraph* Graph : Graphs)
		for (UEdGraphNode* Node : Graph->Nodes)
			for (UEdGraphPin* Pin : Node->Pins)
				if (Pin && Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Text && !UE427IsEmpty(Pin->DefaultValue))
				{
					FText Value;
					if (FTextStringHelper::ReadFromBuffer(*Pin->DefaultValue, Value)) Pin->DefaultTextValue = Value;
					else Pin->DefaultTextValue = FText::FromString(Pin->DefaultValue);
					Pin->DefaultValue.Reset();
				}
    for (UEdGraph* Graph : Graphs)
        for (UEdGraphNode* Node : Graph->Nodes)
            if (auto Call = Cast<UK2Node_CallFunction>(Node))
                if (Call->FunctionReference.GetMemberParentClass() == SignatureOwner.Get())
                    Call->FunctionReference.SetSelfMember(Call->FunctionReference.GetMemberName());
	for (UEdGraph* Graph : Graphs) RestoreFormatTextNodes(Graph);
	for (UEdGraph* Graph : Graphs)
	{
		const auto Nodes = Graph->Nodes;
		for (UEdGraphNode* Node : Nodes)
			if (auto Getter = Cast<UK2Node_VariableGet>(Node))
				for (UTimelineTemplate* Template : Blueprint->Timelines)
				{
					if (!Template) continue;
					FName TrackName;
					if (Template->GetDirectionPropertyName() == Getter->GetVarName()) TrackName = TEXT("Direction");
					for (const auto& Track : Template->FloatTracks) if (Track.GetPropertyName() == Getter->GetVarName()) TrackName = Track.GetTrackName();
					for (const auto& Track : Template->VectorTracks) if (Track.GetPropertyName() == Getter->GetVarName()) TrackName = Track.GetTrackName();
					for (const auto& Track : Template->LinearColorTracks) if (Track.GetPropertyName() == Getter->GetVarName()) TrackName = Track.GetTrackName();
					if (TrackName.IsNone()) continue;
					for (UEdGraphNode* Candidate : Nodes)
						if (auto Timeline = Cast<UK2Node_Timeline>(Candidate); Timeline && Timeline->TimelineName == Template->GetVariableName())
							if (auto Output = Timeline->FindPin(TrackName, EGPD_Output))
							{
								for (auto Link : Getter->GetValuePin()->LinkedTo) Output->MakeLinkTo(Link);
								Getter->BreakAllNodeLinks();
								Graph->RemoveNode(Getter);
								break;
							}
				}
	}
	for (UEdGraph* Graph : Graphs) {
		const auto Nodes = Graph->Nodes;
		for (UEdGraphNode* Node : Nodes) {
			if (auto Assignment = Cast<UK2Node_AssignmentStatement>(Node)) {
				auto Value = Assignment->GetValuePin();
				if (Value->PinType.ContainerType == EPinContainerType::Map && UE427IsEmpty(Value->LinkedTo) && Value->DefaultValue == TEXT("()")) {
					auto EmptyMap = NewObject<UK2Node_MakeMap>(Graph);
					EmptyMap->CreateNewGuid();
					EmptyMap->NumInputs = 0;
					EmptyMap->NodePosX = Node->NodePosX - 200;
					EmptyMap->NodePosY = Node->NodePosY - 150;
					Graph->AddNode(EmptyMap, false, false);
					EmptyMap->AllocateDefaultPins();
					EmptyMap->GetOutputPin()->PinType = Value->PinType;
					EmptyMap->GetOutputPin()->MakeLinkTo(Value);
					Value->DefaultValue.Reset();
				}
			}
		}
	}
	for (UEdGraph* Graph : Graphs)
		for (UEdGraphNode* Node : Graph->Nodes)
			if (auto Call = Cast<UK2Node_CallFunction>(Node))
			{
				const UFunction* Function = Call->GetTargetFunction();
				const UClass* OwnerClass = Function ? Function->GetOwnerClass() : nullptr;
				const UFunction* Signature = OwnerClass && OwnerClass->ClassGeneratedBy == Blueprint ? Reader.Signatures.FindRef(Function->GetName()) : nullptr;
				if (!Signature) continue;
				const auto Pins = Call->Pins;
				for (UEdGraphPin* Pin : Pins)
					if (Pin->PinType.PinCategory != UEdGraphSchema_K2::PC_Exec && Pin->PinName != UEdGraphSchema_K2::PN_Self && !FindFProperty<FProperty>(Signature, Pin->PinName))
					{
						if (UE427IsEmpty(Pin->LinkedTo)) { Call->Pins.Remove(Pin); Pin->MarkPendingKill(); }
						else OutError = FString::Printf(TEXT("Stale connected parameter %s on %s"), *Pin->PinName.ToString(), *Function->GetName());
					}
			}
	for (UEdGraph* Graph : Graphs)
		for (UEdGraphNode* Node : Graph->Nodes)
			if (auto Assignment = Cast<UK2Node_AssignmentStatement>(Node))
			{
				auto Variable = Assignment->GetVariablePin();
				auto Value = Assignment->GetValuePin();
				UE_LOG(LogCASGraph, Verbose, TEXT("Assignment value %s at %d,%d: type %s container %d object %s, variable type %s container %d, linked source %s"), *Graph->GetPathName(), Node->NodePosX, Node->NodePosY, *Value->PinType.PinCategory.ToString(), (int32)Value->PinType.ContainerType, *GetPathNameSafe(Value->PinType.PinSubCategoryObject.Get()), *Variable->PinType.PinCategory.ToString(), (int32)Variable->PinType.ContainerType, UE427IsEmpty(Value->LinkedTo) ? TEXT("none") : *Value->LinkedTo[0]->GetOwningNode()->GetNodeTitle(ENodeTitleType::FullTitle).ToString());
				UE_LOG(LogCASGraph, Verbose, TEXT("Assignment %s at %d,%d: %d variable connections (%s)"), *Graph->GetPathName(), Node->NodePosX, Node->NodePosY, Variable->LinkedTo.Num(), UE427IsEmpty(Variable->LinkedTo) ? TEXT("missing") : *Variable->LinkedTo[0]->GetOwningNode()->GetPathName());
			}
	struct FPreservedPinDefault
	{
		UEdGraphNode* Node;
		FName Name;
		FString Value;
		UObject* Object;
		FText Text;
	};
	TArray<FPreservedPinDefault> PreservedDefaults;
	for (UEdGraph* Graph : Graphs)
		for (UEdGraphNode* Node : Graph->Nodes)
			for (UEdGraphPin* Pin : Node->Pins)
				if (Pin->Direction == EGPD_Input && UE427IsEmpty(Pin->LinkedTo) && (!UE427IsEmpty(Pin->DefaultValue) || Pin->DefaultObject || !UE427IsEmpty(Pin->DefaultTextValue)))
					PreservedDefaults.Add({Node, Pin->PinName, Pin->DefaultValue, Pin->DefaultObject, Pin->DefaultTextValue});
	FBlueprintEditorUtils::RefreshAllNodes(Blueprint);
	for (const auto& Saved : PreservedDefaults)
		if (auto Pin = Saved.Node->FindPin(Saved.Name, EGPD_Input); Pin && UE427IsEmpty(Pin->LinkedTo))
		{
			Pin->DefaultValue = Saved.Value;
			Pin->DefaultObject = Saved.Object;
			Pin->DefaultTextValue = Saved.Text;
		}
	for (UEdGraph* Graph : Graphs)
	{
		const auto Nodes = Graph->Nodes;
		for (UEdGraphNode* Node : Nodes)
			if (auto Assignment = Cast<UK2Node_AssignmentStatement>(Node))
			{
				auto Value = Assignment->GetValuePin();
				auto Enum = Cast<UEnum>(Value->PinType.PinSubCategoryObject.Get());
				if (!Enum || Value->PinType.PinCategory != UEdGraphSchema_K2::PC_Byte || Value->PinType.IsContainer() || !UE427IsEmpty(Value->LinkedTo) || UE427IsEmpty(Value->DefaultValue)) continue;
				if (Enum->GetIndexByName(*Value->DefaultValue) == INDEX_NONE) continue;
				auto Literal = NewObject<UK2Node_EnumLiteral>(Graph);
				Literal->Enum = Enum;
				Literal->CreateNewGuid();
				Literal->NodePosX = Node->NodePosX - 200;
				Literal->NodePosY = Node->NodePosY + 100;
				Graph->AddNode(Literal, false, false);
				Literal->AllocateDefaultPins();
				Literal->FindPinChecked(UK2Node_EnumLiteral::GetEnumInputPinName())->DefaultValue = Value->DefaultValue;
				Literal->FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue)->MakeLinkTo(Value);
			}
	}
	for (UEdGraph* Graph : Graphs)
	{
		const auto Nodes = Graph->Nodes;
		for (UEdGraphNode* Node : Nodes)
			for (UEdGraphPin* Output : Node->Pins)
			{
				if (Output->Direction != EGPD_Output || Output->PinType.PinCategory != UEdGraphSchema_K2::PC_Object || Output->PinType.IsContainer()) continue;
				auto SourceClass = Cast<UClass>(Output->PinType.PinSubCategoryObject.Get());
				const auto Links = Output->LinkedTo;
				for (UEdGraphPin* Input : Links)
				{
					if (Input->PinType.PinCategory != UEdGraphSchema_K2::PC_Object || Input->PinType.IsContainer()) continue;
					auto TargetClass = Cast<UClass>(Input->PinType.PinSubCategoryObject.Get());
					if (!SourceClass || !TargetClass || SourceClass == TargetClass || !TargetClass->IsChildOf(SourceClass)) continue;
					auto CastNode = NewObject<UK2Node_DynamicCast>(Graph);
					CastNode->CreateNewGuid();
					CastNode->TargetType = TargetClass;
					CastNode->SetPurity(true);
					CastNode->NodePosX = Input->GetOwningNode()->NodePosX - 200;
					CastNode->NodePosY = Input->GetOwningNode()->NodePosY - 100;
					Graph->AddNode(CastNode, false, false);
					CastNode->AllocateDefaultPins();
					Output->BreakLinkTo(Input);
					auto Source = CastNode->GetCastSourcePin();
					Source->PinType = Output->PinType;
					Output->MakeLinkTo(Source);
					CastNode->PinConnectionListChanged(Source);
					CastNode->GetCastResultPin()->MakeLinkTo(Input);
				}
			}
	}
	int32 ConversionCount = 0;
	int32 TimelineInputCount = 0;
	for (UEdGraph* Graph : Graphs) TimelineInputCount += RestoreTimelineInputs(Graph);
	UE_LOG(LogCASGraph, Display, TEXT("Restored %d timeline control inputs for %s"), TimelineInputCount, *Blueprint->GetPathName());
	for (UEdGraph* Graph : Graphs) ConversionCount += RemoveRedundantRealConversions(Graph);
	UE_LOG(LogCASGraph, Display, TEXT("Removed %d redundant real conversion nodes for %s"), ConversionCount, *Blueprint->GetPathName());
	int32 MacroCount = 0;
	for (UEdGraph* Graph : Graphs) MacroCount += FCASMacroReconstruction::Apply(Graph);
	UE_LOG(LogCASGraph, Display, TEXT("Recovered %d standard IsValid macros for %s"), MacroCount, *Blueprint->GetPathName());
	int32 LoopCount = 0;
	for (UEdGraph* Graph : Graphs) LoopCount += FCASMacroReconstruction::ApplyLoops(Graph, UAssetGeneratorLocalSettings::Get()->bAggressiveMacroDetection);
	UE_LOG(LogCASGraph, Display, TEXT("Recovered %d standard loop macros for %s"), LoopCount, *Blueprint->GetPathName());
	UE_LOG(LogCASGraph, Display, TEXT("Reconstructed %d Kismet function bodies for %s (%d builder warnings)"), Functions.Num(), *Blueprint->GetPathName(), Builder.GetWarnings().Num());
	for (UEdGraph* Graph : Graphs)
		for (UEdGraphNode* Node : Graph->Nodes)
			for (UEdGraphPin* Pin : Node->Pins)
				if (Pin->Direction == EGPD_Input && Pin->PinType.ContainerType == EPinContainerType::Map)
					UE_LOG(LogCASGraph, Verbose, TEXT("Map input %s %s.%s default=%s links=%d source=%s"), *Graph->GetName(), *Node->GetNodeTitle(ENodeTitleType::ListView).ToString(), *Pin->PinName.ToString(), *Pin->DefaultValue, Pin->LinkedTo.Num(), UE427IsEmpty(Pin->LinkedTo) ? TEXT("none") : *Pin->LinkedTo[0]->GetOwningNode()->GetNodeTitle(ENodeTitleType::ListView).ToString());
	if (AnimationData)
		for (const auto& Node : (*AnimationData)->GetArrayField(TEXT("Nodes")))
			for (const TCHAR* Field : {TEXT("InitialUpdateFunction"), TEXT("BecomeRelevantFunction"), TEXT("UpdateFunction")})
			{
				const TSharedPtr<FJsonObject>* Reference = nullptr;
				if (!Node->AsObject()->GetObjectField(TEXT("Properties"))->TryGetObjectField(Field, Reference)) continue;
				const FString Name = String(*Reference, TEXT("FunctionName"));
				for (UEdGraph* Graph : Graphs)
					if (Graph->GetName() == Name)
						for (UEdGraphNode* Entry : Graph->Nodes)
							if (auto FunctionEntry = Cast<UK2Node_FunctionEntry>(Entry)) (void)0;
			}
	if (AnimationData)
	{
		TArray<UEdGraph*> CallbackGraphs;
		Blueprint->GetAllGraphs(CallbackGraphs);
		for (UEdGraph* Graph : CallbackGraphs)
			for (UEdGraphNode* Node : Graph->Nodes)
			{
				if (auto Event = Cast<UK2Node_CustomEvent>(Node))
					if (Event->CustomFunctionName.ToString().StartsWith(TEXT("Recovered_AnimNode_CallFunction_EventStub"))) (void)0;
				if (auto Entry = Cast<UK2Node_FunctionEntry>(Node))
					if (Graph->GetName().StartsWith(TEXT("Recovered_AnimNode_CallFunction_EventStub"))) (void)0;
			}
	}
	if (AnimationData && !AnimationGraphReconstruction::RestorePropertyAccesses(CastChecked<UAnimBlueprint>(Blueprint), *AnimationData, OutError)) return false;
	if (AnimationData)
	{
		TSet<UEdGraph*> ThreadSafeGraphs;
		TArray<UEdGraph*> Pending;
		for (UEdGraph* Graph : Graphs)
			for (UEdGraphNode* Node : Graph->Nodes)
				if (auto Entry = Cast<UK2Node_FunctionEntry>(Node); Entry &&
					(false || Graph->GetFName() == TEXT("BlueprintThreadSafeUpdateAnimation")))
				{
					(void)0;
					ThreadSafeGraphs.Add(Graph);
					Pending.AddUnique(Graph);
				}
		while (!UE427IsEmpty(Pending))
		{
			UEdGraph* Graph = Pending.Pop();
			for (UEdGraphNode* Node : Graph->Nodes)
				if (auto Call = Cast<UK2Node_CallFunction>(Node); Call && Call->FunctionReference.IsSelfContext())
					for (UEdGraph* Callee : Graphs)
						if (Callee->GetFName() == Call->FunctionReference.GetMemberName() && !ThreadSafeGraphs.Contains(Callee))
							for (UEdGraphNode* CalleeNode : Callee->Nodes)
								if (auto Entry = Cast<UK2Node_FunctionEntry>(CalleeNode))
								{
									(void)0;
									ThreadSafeGraphs.Add(Callee);
									Pending.AddUnique(Callee);
								}
		}
	}
	if (!SkippedFunction && UE427IsEmpty(Builder.GetWarnings())) Metadata->SetValue(Blueprint, TEXT("CASGraphSource"), *SourceHash);
	return true;
}

void FCASJsonGraph::CollectDependencies(const TSharedPtr<FJsonObject>& AssetData, TArray<FString>& Packages)
{
	TFunction<void(const TSharedPtr<FJsonValue>&, const FString&, int32)> Visit;
	Visit = [&](const TSharedPtr<FJsonValue>& Value, const FString& Key, int32 Depth)
	{
		if (!Value || Depth > 128) return;
		if (Value->Type == EJson::Object)
			for (const auto& Pair : Value->AsObject()->Values) Visit(Pair.Value, Pair.Key, Depth + 1);
		else if (Value->Type == EJson::Array)
			for (const auto& Item : Value->AsArray()) Visit(Item, Key, Depth + 1);
		else if (Value->Type == EJson::String && (Key == TEXT("TableId") || Key == TEXT("Object") || Key == TEXT("Class") || Key == TEXT("Struct") || Key == TEXT("InterfaceClass") || Key == TEXT("ObjectClass") || Key == TEXT("ContextClass") || Key == TEXT("FunctionOwner") || Key == TEXT("PinSubCategoryObject") || Key == TEXT("MemberParent")))
		{
			const FString Path = Value->AsString();
			const FString Package = FPackageName::ObjectPathToPackageName(Path);
			if (Package.StartsWith(TEXT("/")) && !Package.StartsWith(TEXT("/Script/")) && FPackageName::IsValidLongPackageName(Package)) Packages.AddUnique(Package);
		}
	};
	if (const auto Children = AssetData->TryGetField(TEXT("Children"))) Visit(Children, TEXT("Children"), 0);
}



