#include "CoreMinimal.h"
#if ENGINE_MAJOR_VERSION >= 5
#include "ControlRigGraphReconstruction.h"
#include "UE427AssetPath.h"
#include "Misc/Parse.h"
#include "ControlRigBlueprint.h"
#include "RigVMModel/RigVMController.h"
#include "RigVMModel/RigVMNode.h"
#include "RigVMModel/RigVMPin.h"
#include "Dom/JsonObject.h"

namespace ControlRigGraphReconstruction
{
bool RestoreExecutionLinks(UControlRigBlueprint* Blueprint, const TSharedPtr<FJsonObject>& Data,
    const TMap<int32, URigVMNode*>& Nodes, TArray<FString>& Errors)
{
    auto Controller = Blueprint->GetOrCreateController();
    const auto VM = Data->GetObjectField(TEXT("VM"));
    const auto Code = VM->GetObjectField(TEXT("ByteCodeStorage"));
    const auto& Instructions = Code->GetArrayField(TEXT("Instructions"));
    const auto& Branches = Code->GetArrayField(TEXT("BranchInfos"));
    const auto& Functions = VM->GetArrayField(TEXT("FunctionNamesStorage"));
    TSet<int32> Visited;
    auto Connect = [&](URigVMPin* From, URigVMPin* To)
    {
        if (!From || !To || !Controller->AddLink(From->GetPinPath(), To->GetPinPath(), false))
        { Errors.Add(FString::Printf(TEXT("Cannot restore rig execution link: %s -> %s."), From ? *From->GetPinPath() : TEXT("None"), To ? *To->GetPinPath() : TEXT("None"))); return false; }
        return true;
    };
    TFunction<bool(int32, int32, int32, URigVMPin*)> Region;
    Region = [&](int32 First, int32 Last, int32 ReturnTarget, URigVMPin* Previous)
    {
        if (First == Last + 1) return true;
        if (!Instructions.IsValidIndex(First) || !Instructions.IsValidIndex(Last) || First > Last)
        { Errors.Add(TEXT("Invalid recovered execution region.")); return false; }
        for (int32 Index = First; Index <= Last; ++Index)
        {
            if (Visited.Contains(Index)) { Errors.Add(TEXT("Overlapping recovered execution regions.")); return false; }
            Visited.Add(Index);
            const auto Instruction = Instructions[Index]->AsObject();
            const int32 Opcode = Instruction->GetIntegerField(TEXT("OpCode"));
            if (Opcode == 65) continue;
            if (Opcode == 68)
            {
                if (auto Setter = Nodes.FindRef(Index))
                {
                    auto Execute = Setter->FindPin(TEXT("ExecuteContext"));
                    if (!Connect(Previous, Execute)) return false;
                    Previous = Execute;
                }
                continue;
            }
            if (Opcode == 80)
            {
                if (Index != Last) { Errors.Add(TEXT("Unexpected rig exit inside an execution region.")); return false; }
                return true;
            }
            if (Opcode == 75)
            {
                if (Index != Last || Index - Instruction->GetIntegerField(TEXT("InstructionIndex")) != ReturnTarget)
                { Errors.Add(TEXT("Unsupported backward rig execution jump.")); return false; }
                return true;
            }
            if (Opcode == 74 && Instruction->GetIntegerField(TEXT("InstructionIndex")) == 1) continue;
            if (Opcode != 101 || !Nodes.Contains(Index)) { Errors.Add(FString::Printf(TEXT("Unsupported execution instruction %d at %d."), Opcode, Index)); return false; }
            auto Node = Nodes[Index];
            const FString Function = Functions[Instruction->GetIntegerField(TEXT("FunctionIndex"))]->AsString();
            if (Function == TEXT("FRigVMFunction_Sequence::Execute") || Function == TEXT("FRigUnit_SequenceExecution::Execute")) continue;
            auto Input = Node->FindPin(TEXT("ExecuteContext"));
            if (!Input || !Input->IsExecuteContext()) continue;
            if (Node->IsEvent())
            {
                if (Previous || Index != First || ReturnTarget != INDEX_NONE) { Errors.Add(TEXT("Unexpected rig entry node.")); return false; }
                Previous = Input;
                continue;
            }
            if (!Connect(Previous, Input)) return false;
            if (Instructions.IsValidIndex(Index + 1) && Instructions[Index + 1]->AsObject()->GetIntegerField(TEXT("OpCode")) == 100)
            {
                if (!Instructions.IsValidIndex(Index + 1) || Instructions[Index + 1]->AsObject()->GetIntegerField(TEXT("OpCode")) != 100)
                { Errors.Add(TEXT("Missing saved rig branch dispatch.")); return false; }
                const auto Dispatch = Instructions[Index + 1]->AsObject();
                const int32 BranchStart = Dispatch->GetIntegerField(TEXT("FirstBranchInfoIndex"));
                const int32 BranchInstruction = Index;
                int32 Next = Index + 2;
                Visited.Add(Index + 1);
                for (int32 Block = 0; Block < 3; ++Block)
                {
                    if (!Branches.IsValidIndex(BranchStart + Block)) { Errors.Add(TEXT("Missing saved rig branch block.")); return false; }
                    const auto Branch = Branches[BranchStart + Block]->AsObject();
                    const FString Label = Branch->GetStringField(TEXT("Label"));
                    const FString Expected = Block == 0 ? TEXT("True") : Block == 1 ? TEXT("False") : TEXT("Completed");
                    const int32 Start = Branch->GetIntegerField(TEXT("FirstInstruction"));
                    const int32 End = Branch->GetIntegerField(TEXT("LastInstruction"));
                    if (Label != Expected || Branch->GetIntegerField(TEXT("InstructionIndex")) != BranchInstruction + 1 || Start != Next || End > Last)
                    { Errors.Add(TEXT("Unsupported rig branch layout.")); return false; }
                    if (Block == 2)
                    {
                        if (End != Start - 1) { Errors.Add(TEXT("Nonempty completed block needs additional rig recovery.")); return false; }
                    }
                    else if (!Region(Start, End, BranchInstruction, Node->FindPin(Label))) return false;
                    Next = End + 1;
                }
                Previous = Node->FindPin(TEXT("Completed"));
                Index = Next - 1;
                continue;
            }
            if (Input->GetDirection() != ERigVMPinDirection::IO) { Errors.Add(TEXT("Unsupported mutable rig execution signature: ") + Function); return false; }
            Previous = Input;
        }
        return true;
    };
    TArray<int32> Starts;
    Starts.Add(0);
    for (const auto& Entry : Code->GetArrayField(TEXT("Entries")))
    {
        int32 Start = 0;
        if (Entry->Type == EJson::String) FParse::Value(*Entry->AsString(), TEXT("InstructionIndex="), Start);
        else if (Entry->Type == EJson::Object) Start = Entry->AsObject()->GetIntegerField(TEXT("InstructionIndex"));
        Starts.AddUnique(Start);
    }
    Starts.Sort();
    for (int32 Index = 0; Index < Starts.Num(); ++Index)
        if (!Region(Starts[Index], Index + 1 < Starts.Num() ? Starts[Index + 1] - 1 : Instructions.Num() - 1, INDEX_NONE, nullptr)) return false;
    return UE427IsEmpty(Errors);
}
}

#endif
