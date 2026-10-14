#pragma once

#include "CoreMinimal.h"
#include "UE427AssetPath.h"
#include "UObject/Script.h"

class UFunction;
class UClass;
class UScriptStruct;

struct FCASDecompiledExpr : public TSharedFromThis<FCASDecompiledExpr>
{
	EExprToken Token;
	int32 StartOffset = 0; 
	int32 EndOffset = 0;   

	UFunction* FunctionRef = nullptr;
	FProperty* PropertyRef = nullptr;
	FField* FieldRef = nullptr;
	UObject* ObjectRef = nullptr;
	UClass* ClassRef = nullptr;
	UScriptStruct* StructRef = nullptr;

	int32 IntValue = 0;
	int64 Int64Value = 0;
	uint8 ByteValue = 0;
	double FloatValue = 0.0;
	FString StringValue;
	FName NameValue;
	bool BoolValue = false;
	FVector VectorValue = FVector::ZeroVector;
	FRotator RotatorValue = FRotator::ZeroRotator;

	uint32 JumpTarget = 0; 

	TArray<TSharedPtr<FCASDecompiledExpr>> Children; 
	TSharedPtr<FCASDecompiledExpr> ContextObject;    

	bool IsLiteral() const;

	FString GetLiteralAsString() const;

	bool IsStatement() const;

	bool IsFunctionCall() const;

	FString ToDebugString() const;
};

struct FCASDecompiledFunction
{
	UFunction* OriginalFunction = nullptr;
	FString FunctionName;

	TArray<TSharedPtr<FCASDecompiledExpr>> Statements;

	TMap<int32, int32> OffsetToStatementIndex;

	TMap<FName, int32> EventEntryPoints;

	bool bIsUbergraph = false;
	bool bIsEventStub = false;
	int32 UbergraphEntryOffset = -1; 
};

