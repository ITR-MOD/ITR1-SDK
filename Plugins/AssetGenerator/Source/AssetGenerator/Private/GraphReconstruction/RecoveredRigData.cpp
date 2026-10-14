#include "CoreMinimal.h"
#if ENGINE_MAJOR_VERSION >= 5
#include "ControlRigGraphReconstruction.h"
#include "UE427AssetPath.h"
#include "Misc/Parse.h"
#include "ControlRigBlueprint.h"
#include "RigVMCore/RigVMRegistry.h"
#include "RigVMModel/RigVMController.h"
#include "RigVMModel/RigVMPin.h"
#include "RigVMModel/Nodes/RigVMVariableNode.h"
#include "Dom/JsonObject.h"
#include "UObject/UnrealType.h"

namespace ControlRigGraphReconstruction
{
namespace
{
struct FRecoveredValue
{
    URigVMPin* Pin = nullptr;
    TSharedPtr<FJsonValue> Literal;
    TMap<FString, TSharedPtr<FRecoveredValue>> Children;
};
using FValue = TSharedPtr<FRecoveredValue>;

FValue ChildValue(const FValue& Value, const FString& Name)
{
    if (!Value) return nullptr;
    if (const auto* Override = Value->Children.Find(Name)) return *Override;
    auto Result = MakeShared<FRecoveredValue>();
    if (Value->Pin) Result->Pin = Value->Pin->FindSubPin(Name);
    if (Value->Literal)
    {
        if (Value->Literal->Type == EJson::Object)
        {
            if (const auto* Field = Value->Literal->AsObject()->Values.Find(Name)) Result->Literal = *Field;
        }
        else if (Value->Literal->Type == EJson::Array && Name.IsNumeric())
        {
            const auto& Array = Value->Literal->AsArray();
            const int32 Index = FCString::Atoi(*Name);
            if (Array.IsValidIndex(Index)) Result->Literal = Array[Index];
        }
    }
    if (Result->Pin || Result->Literal) return Result;
    return nullptr;
}

FValue ReplaceValue(const FValue& Original, const TArray<FString>& Path, int32 Index, const FValue& Replacement)
{
    if (Index == Path.Num()) return Replacement;
    auto Result = Original ? MakeShared<FRecoveredValue>(*Original) : MakeShared<FRecoveredValue>();
    Result->Children.Add(Path[Index], ReplaceValue(ChildValue(Original, Path[Index]), Path, Index + 1, Replacement));
    return Result;
}
}

bool RestoreDataLinks(UControlRigBlueprint* Blueprint, const TSharedPtr<FJsonObject>& Data,
    TMap<int32, URigVMNode*>& Nodes, TArray<FString>& Errors)
{
    auto Controller = Blueprint->GetOrCreateController();
    const auto VM = Data->GetObjectField(TEXT("VM"));
    const auto Code = VM->GetObjectField(TEXT("ByteCodeStorage"));
    const auto& Instructions = Code->GetArrayField(TEXT("Instructions"));
    const auto& Functions = VM->GetArrayField(TEXT("FunctionNamesStorage"));
    const auto& Variables = Data->GetArrayField(TEXT("PublicVariables"));
    TMap<int32, FValue> Initial;
    const auto Work = VM->GetObjectField(TEXT("DefaultWorkMemoryStorage"));
    const auto& Descs = Work->GetArrayField(TEXT("PropertyDescs"));
    for (int32 Index = 0; Index < Descs.Num(); ++Index)
    {
        const FString Name = Descs[Index]->AsObject()->GetStringField(TEXT("Name"));
        const auto* Default = Work->GetObjectField(TEXT("Values"))->Values.Find(Name);
        if (Default)
        {
            auto Value = MakeShared<FRecoveredValue>();
            Value->Literal = *Default;
            Initial.Add(Index, Value);
        }
    }
    auto PathFor = [&](const TSharedPtr<FJsonObject>& Operand, TArray<FString>& Path)
    {
        const int32 Offset = Operand->GetIntegerField(TEXT("RegisterOffset"));
        if (Offset == 65535) return true;
        const int32 Memory = Operand->GetIntegerField(TEXT("MemoryType"));
        const TArray<TSharedPtr<FJsonValue>>* Paths = nullptr;
        if (Memory == 0) Paths = &Work->GetArrayField(TEXT("PropertyPathDescriptions"));
        else if (Memory == 2) Paths = &VM->GetArrayField(TEXT("ExternalPropertyPathDescriptions"));
        if (!Paths || !Paths->IsValidIndex(Offset)) return false;
        const auto Description = (*Paths)[Offset]->AsObject();
        Description->GetStringField(TEXT("SegmentPath")).ParseIntoArray(Path, TEXT("."));
        return true;
    };
    int32 VariableSerial = 0;
    auto VariableNode = [&](int32 Register, bool Getter) -> URigVMVariableNode*
    {
        if (!Variables.IsValidIndex(Register)) return nullptr;
        const FString Name = Variables[Register]->AsObject()->GetStringField(TEXT("Name"));
        FProperty* Property = Blueprint->GeneratedClass->FindPropertyByName(*Name);
        if (!Property) return nullptr;
        UObject* TypeObject = nullptr;
        if (auto Struct = CastField<FStructProperty>(Property)) TypeObject = Struct->Struct;
        return Controller->AddVariableNode(*Name, Property->GetCPPType(), TypeObject, Getter, TEXT(""),
            FVector2D(-600, VariableSerial++ * 180), FString(), false);
    };
    auto Read = [&](const TSharedPtr<FJsonObject>& Operand, const TMap<int32, FValue>& State) -> FValue
    {
        const int32 Memory = Operand->GetIntegerField(TEXT("MemoryType"));
        const int32 Register = Operand->GetIntegerField(TEXT("RegisterIndex"));
        FValue Value;
        if (Memory == 0) Value = State.FindRef(Register);
        else if (Memory == 1)
        {
            const auto Literal = VM->GetObjectField(TEXT("LiteralMemoryStorage"));
            const auto& Properties = Literal->GetArrayField(TEXT("PropertyDescs"));
            if (!Properties.IsValidIndex(Register)) return nullptr;
            const auto* Saved = Literal->GetObjectField(TEXT("Values"))->Values.Find(Properties[Register]->AsObject()->GetStringField(TEXT("Name")));
            if (!Saved) return nullptr;
            Value = MakeShared<FRecoveredValue>();
            Value->Literal = *Saved;
        }
        else if (Memory == 2)
        {
            auto Node = VariableNode(Register, true);
            if (!Node) return nullptr;
            Value = MakeShared<FRecoveredValue>();
            Value->Pin = Node->GetValuePin();
        }
        TArray<FString> Path;
        if (!PathFor(Operand, Path)) return nullptr;
        for (const auto& Segment : Path) Value = ChildValue(Value, Segment);
        return Value;
    };
    TFunction<bool(const FValue&, URigVMPin*)> Apply;
    Apply = [&](const FValue& Value, URigVMPin* Pin)
    {
        if (!Value || !Pin) return false;
        if (UE427IsEmpty(Value->Children))
        {
            if (Value->Pin) return Controller->AddLink(Value->Pin->GetPinPath(), Pin->GetPinPath(), false);
            FString Error;
            const bool Result = Value->Literal && RestorePinValue(Controller, Pin, Value->Literal, Error);
            if (!Result && !UE427IsEmpty(Error)) Errors.Add(Error);
            return Result;
        }
        if (Pin->IsArray() && Value->Literal && Value->Literal->Type == EJson::Array)
        {
            const int32 Size = Value->Literal->AsArray().Num();
            if (Pin->GetSubPins().Num() != Size && !Controller->SetArrayPinSize(Pin->GetPinPath(), Size, TEXT(""), false)) return false;
        }
        if (UE427IsEmpty(Pin->GetSubPins())) return false;
        for (auto Child : Pin->GetSubPins())
            if (!Apply(ChildValue(Value, Child->GetName()), Child)) return false;
        return true;
    };
    auto Write = [&](const TSharedPtr<FJsonObject>& Operand, const FValue& Value, TMap<int32, FValue>& State, int32 Instruction)
    {
        TArray<FString> Path;
        if (!PathFor(Operand, Path)) return false;
        const int32 Register = Operand->GetIntegerField(TEXT("RegisterIndex"));
        const int32 Memory = Operand->GetIntegerField(TEXT("MemoryType"));
        if (Memory == 0)
        {
            State.Add(Register, ReplaceValue(State.FindRef(Register), Path, 0, Value));
            return true;
        }
        if (Memory == 2 && UE427IsEmpty(Path))
        {
            auto Setter = VariableNode(Register, false);
            if (!Setter || Nodes.Contains(Instruction) || !Apply(Value, Setter->GetValuePin())) return false;
            Nodes.Add(Instruction, Setter);
            return true;
        }
        return false;
    };
    TFunction<bool(int32, int32, TMap<int32, FValue>&)> Region;
    Region = [&](int32 First, int32 Last, TMap<int32, FValue>& State)
    {
        for (int32 Index = First; Index <= Last; ++Index)
        {
            const auto Op = Instructions[Index]->AsObject();
            const int32 Opcode = Op->GetIntegerField(TEXT("OpCode"));
            if (Opcode == 68)
            {
                const auto Value = Read(Op->GetObjectField(TEXT("Source")), State);
                if (!Value || !Write(Op->GetObjectField(TEXT("Target")), Value, State, Index))
                { Errors.Add(FString::Printf(TEXT("Cannot recover register copy at %d."), Index)); return false; }
                continue;
            }
            if (Opcode == 65 || Opcode == 74 || Opcode == 75 || Opcode == 80) continue;
            if (Opcode != 101) { Errors.Add(TEXT("Unexpected data-flow opcode.")); return false; }
            auto Node = Nodes.FindRef(Index);
            if (!Node) return false;
            const FString FunctionName = Functions[Op->GetIntegerField(TEXT("FunctionIndex"))]->AsString();
            const auto Function = FRigVMRegistry::Get().FindFunction(*FunctionName);
            const auto& Operands = Op->GetArrayField(TEXT("Arguments"));
            TArray<TPair<TSharedPtr<FJsonObject>, FValue>> Outputs;
            for (int32 Arg = 0; Arg < Operands.Num(); ++Arg)
            {
                const auto Operand = Operands[Arg]->AsObject();
                FString PinName;
                if (FunctionName.StartsWith(TEXT("DISPATCH_RigVMDispatch_ArrayMake::")))
                    PinName = Arg == Operands.Num() - 1 ? TEXT("Array") : FString::Printf(TEXT("Values.%d"), Arg);
                else PinName = Function->GetArgumentNameForOperandIndex(Arg, Operands.Num()).ToString();
                auto Pin = Node->FindPin(PinName);
                if (!Pin) { Errors.Add(TEXT("Missing data operand pin: ") + PinName); return false; }
                const auto Direction = Pin->GetDirection();
                if (Direction == ERigVMPinDirection::Hidden || Pin->IsExecuteContext()) continue;
                if (Direction == ERigVMPinDirection::Input || Direction == ERigVMPinDirection::IO)
                {
                    if (Operand->GetIntegerField(TEXT("MemoryType")) != 1 && !Apply(Read(Operand, State), Pin))
                    { Errors.Add(FString::Printf(TEXT("Cannot restore data input at %d: %s."), Index, *PinName)); return false; }
                }
                if (Direction == ERigVMPinDirection::Output || Direction == ERigVMPinDirection::IO)
                {
                    auto Value = MakeShared<FRecoveredValue>();
                    Value->Pin = Pin;
                    Outputs.Emplace(Operand, Value);
                }
            }
            for (const auto& Output : Outputs)
                if (!Write(Output.Key, Output.Value, State, Index)) { Errors.Add(TEXT("Cannot restore data output.")); return false; }
            if (Instructions.IsValidIndex(Index + 1) && Instructions[Index + 1]->AsObject()->GetIntegerField(TEXT("OpCode")) == 100)
            {
                const auto Dispatch = Instructions[Index + 1]->AsObject();
                const int32 Start = Dispatch->GetIntegerField(TEXT("FirstBranchInfoIndex"));
                const auto& Branches = Code->GetArrayField(TEXT("BranchInfos"));
                auto TrueState = State, FalseState = State;
                const auto TrueBranch = Branches[Start]->AsObject(), FalseBranch = Branches[Start + 1]->AsObject();
                if (!Region(TrueBranch->GetIntegerField(TEXT("FirstInstruction")), TrueBranch->GetIntegerField(TEXT("LastInstruction")), TrueState) ||
                    !Region(FalseBranch->GetIntegerField(TEXT("FirstInstruction")), FalseBranch->GetIntegerField(TEXT("LastInstruction")), FalseState)) return false;
                for (auto& Pair : State)
                    Pair.Value = TrueState.FindRef(Pair.Key) == FalseState.FindRef(Pair.Key) ? TrueState.FindRef(Pair.Key) : nullptr;
                Index = FalseBranch->GetIntegerField(TEXT("LastInstruction"));
            }
        }
        return true;
    };
    return Region(0, Instructions.Num() - 1, Initial) && UE427IsEmpty(Errors);
}
}

#endif
