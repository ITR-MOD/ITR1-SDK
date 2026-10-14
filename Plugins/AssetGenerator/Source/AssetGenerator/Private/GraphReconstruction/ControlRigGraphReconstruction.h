#pragma once

#include "CoreMinimal.h"
#include "UE427AssetPath.h"

class UControlRigBlueprint;
class URigVMNode;
class FJsonObject;
class FJsonValue;
class URigVMController;
class URigVMPin;

namespace ControlRigGraphReconstruction
{
bool BuildAssets(const TSharedPtr<FJsonObject>& AnimationData, FString& Error);
bool RestorePinValue(URigVMController* Controller, URigVMPin* Pin, const TSharedPtr<FJsonValue>& Value, FString& Error);
bool RestoreDataLinks(UControlRigBlueprint* Blueprint, const TSharedPtr<FJsonObject>& Data,
    TMap<int32, URigVMNode*>& Nodes, TArray<FString>& Errors);
bool RestoreVariables(UControlRigBlueprint* Blueprint, const TSharedPtr<FJsonObject>& Data, TArray<FString>& Errors);
bool RestoreHierarchy(UControlRigBlueprint* Blueprint, const TSharedPtr<FJsonObject>& Data, TArray<FString>& Errors);
bool CreateInstructionNodes(UControlRigBlueprint* Blueprint, const TSharedPtr<FJsonObject>& Data,
    TMap<int32, URigVMNode*>& Nodes, TArray<FString>& Errors);
bool RestoreExecutionLinks(UControlRigBlueprint* Blueprint, const TSharedPtr<FJsonObject>& Data,
    const TMap<int32, URigVMNode*>& Nodes, TArray<FString>& Errors);
}
