#include "CoreMinimal.h"
#if ENGINE_MAJOR_VERSION >= 5
#include "ControlRigGraphReconstruction.h"
#include "UE427AssetPath.h"
#include "ControlRigBlueprint.h"
#include "RigVMCore/RigVMRegistry.h"
#include "RigVMCore/RigVMTemplate.h"
#include "RigVMModel/RigVMController.h"
#include "RigVMModel/RigVMGraph.h"
#include "RigVMModel/RigVMPin.h"
#include "RigVMModel/Nodes/RigVMUnitNode.h"
#include "RigVMModel/Nodes/RigVMTemplateNode.h"
#include "Dom/JsonObject.h"
#include "Modules/ModuleManager.h"
#include "JsonObjectConverter.h"
#include "UObject/StructOnScope.h"

namespace ControlRigGraphReconstruction
{
bool RestorePinValue(URigVMController* Controller, URigVMPin* Pin, const TSharedPtr<FJsonValue>& Value, FString& Error)
{
    if (Value->Type == EJson::Array)
    {
        if (!Pin->IsArray()) return false;
        const auto& Values = Value->AsArray();
        if (Pin->GetSubPins().Num() != Values.Num() && !Controller->SetArrayPinSize(Pin->GetPinPath(), Values.Num(), TEXT(""), false)) return false;
        for (int32 Index = 0; Index < Values.Num(); ++Index)
            if (!RestorePinValue(Controller, Pin->GetSubPins()[Index], Values[Index], Error)) return false;
        return true;
    }
    if (Value->Type == EJson::Object)
    {
        if (UE427IsEmpty(Pin->GetSubPins()))
        {
            auto Struct = Cast<UScriptStruct>(Pin->GetCPPTypeObject());
            if (!Struct) { Error = Pin->GetPinPath() + TEXT(" has no struct type"); return false; }
            FStructOnScope Storage(Struct);
            if (!FJsonObjectConverter::JsonObjectToUStruct(Value->AsObject().ToSharedRef(), Struct, Storage.GetStructMemory(), 0, 0))
            { Error = TEXT("Invalid struct literal: ") + Pin->GetPinPath(); return false; }
            FString Text;
            Struct->ExportText(Text, Storage.GetStructMemory(), nullptr, nullptr, PPF_None, nullptr);
            if (!Controller->SetPinDefaultValue(Pin->GetPinPath(), Text, true, false)) { Error = TEXT("Rejected struct literal: ") + Pin->GetPinPath(); return false; }
            return true;
        }
        const auto Object = Value->AsObject();
        for (URigVMPin* Child : Pin->GetSubPins())
        {
            const auto* ChildValue = Object->Values.Find(Child->GetName());
            if (!ChildValue) { Error = TEXT("Missing field ") + Child->GetPinPath(); return false; }
            if (!RestorePinValue(Controller, Child, *ChildValue, Error)) return false;
        }
        return true;
    }
    FString Text;
    switch (Value->Type)
    {
    case EJson::Null: Text = TEXT("None"); break;
    case EJson::Boolean: Text = Value->AsBool() ? TEXT("True") : TEXT("False"); break;
    case EJson::Number: Text = FString::Printf(TEXT("%.17g"), Value->AsNumber()); break;
    case EJson::String: Text = Value->AsString(); break;
    default: return false;
    }
    if (auto Enum = Cast<UEnum>(Pin->GetCPPTypeObject()))
    {
        const int64 EnumValue = Value->Type == EJson::Number ? (int64)Value->AsNumber() : Enum->GetValueByNameString(Text);
        if (!Enum->IsValidEnumValue(EnumValue)) { Error = TEXT("Invalid enum literal: ") + Text; return false; }
        Text = Enum->GetNameStringByValue(EnumValue);
        TGuardValue<bool> AllowHiddenValues(Controller->bValidatePinDefaults, false);
        return Controller->SetPinDefaultValue(Pin->GetPinPath(), Text, true, false);
    }
    if (!Controller->SetPinDefaultValue(Pin->GetPinPath(), Text, true, false)) { Error = TEXT("Rejected value ") + Text + TEXT(" on ") + Pin->GetPinPath(); return false; }
    return true;
}

bool CreateInstructionNodes(UControlRigBlueprint* Blueprint, const TSharedPtr<FJsonObject>& Data,
    TMap<int32, URigVMNode*>& Nodes, TArray<FString>& Errors)
{
    FModuleManager::Get().LoadModule(TEXT("FullBodyIK"));
    Blueprint->InitializeModelIfRequired(false);
    URigVMController* Controller = Blueprint->GetOrCreateController();
    if (!Controller) { Errors.Add(TEXT("Cannot create RigVM model controller.")); return false; }
    const auto VM = Data->GetObjectField(TEXT("VM"));
    const auto& Functions = VM->GetArrayField(TEXT("FunctionNamesStorage"));
    const auto& Instructions = VM->GetObjectField(TEXT("ByteCodeStorage"))->GetArrayField(TEXT("Instructions"));
    for (int32 Index = 0; Index < Instructions.Num(); ++Index)
    {
        const auto Instruction = Instructions[Index]->AsObject();
        const int32 Opcode = Instruction->GetIntegerField(TEXT("OpCode"));
        if (Opcode != 101 && Opcode > 64) continue;
        const int32 FunctionIndex = Instruction->GetIntegerField(TEXT("FunctionIndex"));
        if (!Functions.IsValidIndex(FunctionIndex)) { Errors.Add(FString::Printf(TEXT("Invalid RigVM function index at instruction %d."), Index)); continue; }
        const FString FunctionName = Functions[FunctionIndex]->AsString();
        const FRigVMFunction* Function = FRigVMRegistry::Get().FindFunction(*FunctionName);
        if (!Function) { Errors.Add(TEXT("Missing registered RigVM function: ") + FunctionName); continue; }
        const FString Name = FString::Printf(TEXT("Recovered_%04d"), Index);
        const FVector2D Position((Index % 8) * 400, (Index / 8) * 350);
        URigVMNode* Node = nullptr;
        if (Function->Struct)
            Node = Controller->AddUnitNode(Function->Struct, Function->GetMethodName(), Position, Name, false);
        else if (const FRigVMTemplate* Template = Function->GetTemplate())
        {
            Node = Controller->AddTemplateNode(Template->GetNotation(), Position, Name, false);
            if (Node)
            {
                const auto& Types = Function->GetArgumentTypeIndices();
                const auto& Arguments = Function->GetArguments();
                for (int32 Arg = 0; Arg < Arguments.Num(); ++Arg)
                {
                    URigVMPin* Pin = Node->FindPin(Arguments[Arg].Name);
                    if (Pin && Pin->IsWildCard() && Types.IsValidIndex(Arg))
                        if (!Controller->ResolveWildCardPin(Pin, Types[Arg], false))
                            Errors.Add(TEXT("Cannot resolve rig dispatch pin: ") + Pin->GetPinPath());
                }
            }
        }
        if (!Node) { Errors.Add(TEXT("Cannot recreate RigVM node: ") + FunctionName); continue; }
        Nodes.Add(Index, Node);
        const auto& Operands = Instruction->GetArrayField(TEXT("Arguments"));
        const bool ArrayMake = FunctionName.StartsWith(TEXT("DISPATCH_RigVMDispatch_ArrayMake::"));
        if (ArrayMake)
        {
            auto ValuesPin = Node->FindPin(TEXT("Values"));
            if (!ValuesPin || UE427IsEmpty(Operands) || (ValuesPin->GetSubPins().Num() != Operands.Num() - 1 && !Controller->SetArrayPinSize(ValuesPin->GetPinPath(), Operands.Num() - 1, TEXT(""), false)))
                Errors.Add(TEXT("Cannot restore array-make input count: ") + Name);
        }
        for (int32 OperandIndex = 0; OperandIndex < Operands.Num(); ++OperandIndex)
        {
            const auto Operand = Operands[OperandIndex]->AsObject();
            if (Operand->GetIntegerField(TEXT("MemoryType")) != 1) continue;
            FString PinName;
            if (ArrayMake) PinName = OperandIndex < Operands.Num() - 1 ? FString::Printf(TEXT("Values.%d"), OperandIndex) : TEXT("Array");
            else if (!Function->Factory && !Function->ArgumentNames.IsValidIndex(OperandIndex))
            { Errors.Add(TEXT("RigVM operand count differs from registered function: ") + FunctionName); continue; }
            else PinName = Function->GetArgumentNameForOperandIndex(OperandIndex, Operands.Num()).ToString();
            URigVMPin* Pin = Node->FindPin(PinName);
            if (!Pin) { Errors.Add(TEXT("Cannot find literal operand pin: ") + Name + TEXT(".") + PinName); continue; }
            if (Pin->GetDirection() == ERigVMPinDirection::Output) { Errors.Add(TEXT("Literal memory used as writable output: ") + Pin->GetPinPath()); continue; }
            const auto Memory = VM->GetObjectField(TEXT("LiteralMemoryStorage"));
            const auto& Descriptors = Memory->GetArrayField(TEXT("PropertyDescs"));
            const int32 Register = Operand->GetIntegerField(TEXT("RegisterIndex"));
            if (!Descriptors.IsValidIndex(Register) || Operand->GetIntegerField(TEXT("RegisterOffset")) != 65535)
            { Errors.Add(TEXT("Unsupported literal operand address: ") + Pin->GetPinPath()); continue; }
            const FString Property = Descriptors[Register]->AsObject()->GetStringField(TEXT("Name"));
            const auto* Default = Memory->GetObjectField(TEXT("Values"))->Values.Find(Property);
            FString Error;
            if (!Default || !RestorePinValue(Controller, Pin, *Default, Error)) Errors.Add(TEXT("Cannot restore rig literal: ") + Name + TEXT(".") + PinName + TEXT(" from ") + Property + TEXT(": ") + Error);
        }
    }
    return UE427IsEmpty(Errors);
}
}


#endif
