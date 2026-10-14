#include "BytecodeReader.h"
#include "UE427AssetPath.h"
#include "UObject/Class.h"
#include "UObject/UnrealType.h"
#include "UObject/NameTypes.h"

DEFINE_LOG_CATEGORY_STATIC(LogBytecodeReader, Log, All);

bool FCASDecompiledExpr::IsLiteral() const
{
	switch (Token)
	{
	case EX_StructConst:
		return !UE427IsEmpty(StringValue);
	case EX_SetConst:
		return UE427IsEmpty(Children);
	case EX_IntConst:
	case EX_Int64Const:
	case EX_UInt64Const:
	case EX_DoubleConst:
	case EX_FloatConst:
	case EX_StringConst:
	case EX_UnicodeStringConst:
	case EX_NameConst:
	case EX_ObjectConst:
	case EX_RotationConst:
	case EX_VectorConst:
	case EX_ByteConst:
	case EX_IntConstByte:
	case EX_IntZero:
	case EX_IntOne:
	case EX_True:
	case EX_False:
	case EX_NoObject:
	case EX_TextConst:
		return true;
	default:
		return false;
	}
}

FString FCASDecompiledExpr::GetLiteralAsString() const
{
	switch (Token)
	{
	case EX_StructConst:
		return StringValue;
	case EX_MapConst:
	case EX_SetConst:
		return UE427IsEmpty(Children) ? TEXT("()") : TEXT("");
	case EX_IntConst:
	case EX_IntConstByte:
	case EX_IntZero:
	case EX_IntOne:
		return FString::FromInt(IntValue);
	case EX_Int64Const:
	case EX_UInt64Const:
		return FString::Printf(TEXT("%lld"), Int64Value);
	case EX_DoubleConst:
	case EX_FloatConst:
		return FString::Printf(TEXT("%.17g"), FloatValue);
	case EX_StringConst:
	case EX_UnicodeStringConst:
		return StringValue;
	case EX_NameConst:
		return NameValue.ToString();
	case EX_TextConst:
	{
		const FText Text = ByteValue == 1 && !UE427IsEmpty(Children)
			? FText::ChangeKey(Children[0]->StringValue, NameValue.ToString(), FText::FromString(StringValue))
			: FText::AsCultureInvariant(StringValue);
		FString Buffer;
		FTextStringHelper::WriteToBuffer(Buffer, Text);
		return Buffer;
	}
	case EX_ObjectConst:
		return ObjectRef ? ObjectRef->GetPathName() : TEXT("None");
	case EX_RotationConst:
		return FString::Printf(TEXT("%f,%f,%f"), RotatorValue.Pitch, RotatorValue.Yaw, RotatorValue.Roll);
	case EX_VectorConst:
		return FString::Printf(TEXT("%f,%f,%f"), VectorValue.X, VectorValue.Y, VectorValue.Z);
	case EX_ByteConst:
		return FString::FromInt(ByteValue);
	case EX_True:
		return TEXT("true");
	case EX_False:
		return TEXT("false");
	case EX_NoObject:
		return TEXT("None");
	case EX_Self:
		return TEXT("self");
	default:
		return TEXT("");
	}
}

bool FCASDecompiledExpr::IsStatement() const
{
	switch (Token)
	{
	case EX_Let:
	case EX_LetObj:
	case EX_LetBool:
	case EX_LetDelegate:
	case EX_LetMulticastDelegate:
	case EX_LetWeakObjPtr:
	case EX_LetValueOnPersistentFrame:
	case EX_Jump:
	case EX_JumpIfNot:
	case EX_Return:
	case EX_PushExecutionFlow:
	case EX_PopExecutionFlow:
	case EX_PopExecutionFlowIfNot:
	case EX_EndOfScript:
	case EX_SwitchValue:
	case EX_Assert:
		return true;

	case EX_FinalFunction:
	case EX_LocalFinalFunction:
	case EX_VirtualFunction:
	case EX_LocalVirtualFunction:
	case EX_CallMath:
	case EX_CallMulticastDelegate:
		return true; 

	case EX_BindDelegate:
	case EX_AddMulticastDelegate:
	case EX_RemoveMulticastDelegate:
	case EX_ClearMulticastDelegate:

	case EX_SetArray:
	case EX_SetSet:
	case EX_SetMap:
		return true;
	default:
		return false;
	}
}

bool FCASDecompiledExpr::IsFunctionCall() const
{
	switch (Token)
	{
	case EX_FinalFunction:
	case EX_LocalFinalFunction:
	case EX_VirtualFunction:
	case EX_LocalVirtualFunction:
	case EX_CallMath:
	case EX_CallMulticastDelegate:
		return true;
	default:
		return false;
	}
}

FString FCASDecompiledExpr::ToDebugString() const
{
	FString Result = FString::Printf(TEXT("[%04X] "), StartOffset);

	switch (Token)
	{
	case EX_FinalFunction:
	case EX_LocalFinalFunction:
	case EX_CallMath:
		Result += FString::Printf(TEXT("Call %s (%d args)"),
			FunctionRef ? *FunctionRef->GetName() : TEXT("NULL"), Children.Num());
		break;
	case EX_VirtualFunction:
	case EX_LocalVirtualFunction:
		Result += FString::Printf(TEXT("VirtualCall %s (%d args)"),
			*NameValue.ToString(), Children.Num());
		break;
	case EX_LocalVariable:
		Result += FString::Printf(TEXT("LocalVar %s"), PropertyRef ? *PropertyRef->GetName() : TEXT("NULL"));
		break;
	case EX_InstanceVariable:
		Result += FString::Printf(TEXT("InstanceVar %s"), PropertyRef ? *PropertyRef->GetName() : TEXT("NULL"));
		break;
	case EX_Let:
		Result += TEXT("Let (assign)");
		break;
	case EX_JumpIfNot:
		Result += FString::Printf(TEXT("JumpIfNot -> 0x%04X"), JumpTarget);
		break;
	case EX_Jump:
		Result += FString::Printf(TEXT("Jump -> 0x%04X"), JumpTarget);
		break;
	case EX_Return:
		Result += TEXT("Return");
		break;
	case EX_Context:
		Result += TEXT("Context");
		break;
	case EX_IntConst:
		Result += FString::Printf(TEXT("Int %d"), IntValue);
		break;
	case EX_FloatConst:
		Result += FString::Printf(TEXT("Float %f"), FloatValue);
		break;
	case EX_Self:
		Result += TEXT("Self");
		break;
	case EX_PushExecutionFlow:
		Result += FString::Printf(TEXT("PushExecFlow -> 0x%04X"), JumpTarget);
		break;
	case EX_PopExecutionFlow:
		Result += TEXT("PopExecFlow");
		break;
	case EX_PopExecutionFlowIfNot:
		Result += TEXT("PopExecFlowIfNot");
		break;
	case EX_SwitchValue:
		Result += FString::Printf(TEXT("Switch (%d cases)"), IntValue);
		break;
	case EX_InstanceDelegate:
		Result += FString::Printf(TEXT("InstanceDelegate %s"), *NameValue.ToString());
		break;
	case EX_BindDelegate:
		Result += FString::Printf(TEXT("BindDelegate %s"), *NameValue.ToString());
		break;
	case EX_IntConstByte:
		Result += FString::Printf(TEXT("IntByte %d"), IntValue);
		break;
	case EX_LetObj:
		Result += TEXT("LetObj");
		break;
	case EX_LetBool:
		Result += TEXT("LetBool");
		break;
	case EX_LetWeakObjPtr:
		Result += TEXT("LetWeakObj");
		break;
	case EX_LetDelegate:
		Result += TEXT("LetDelegate");
		break;
	case EX_LetMulticastDelegate:
		Result += TEXT("LetMulticastDelegate");
		break;
	case EX_CallMulticastDelegate:
		Result += FString::Printf(TEXT("CallMulticastDelegate (%d args)"), Children.Num());
		break;
	default:
		Result += FString::Printf(TEXT("Token 0x%02X"), (uint8)Token);
		break;
	}

	return Result;
}
