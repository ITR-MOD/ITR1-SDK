#include "CoreMinimal.h"
#if ENGINE_MAJOR_VERSION >= 5
#include "Misc/AutomationTest.h"
#include "UE427AssetPath.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Misc/FileHelper.h"
#include "Serialization/JsonSerializer.h"
#include "Dom/JsonObject.h"
#include "ControlRigBlueprintFactory.h"
#include "Rigs/RigHierarchy.h"
#include "JsonObjectConverter.h"
#include "UObject/UnrealType.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "ControlRig.h"
#include "RigVMCore/RigVM.h"
#include "Misc/PackageName.h"
#include "UObject/SavePackage.h"
#include "Engine/World.h"
#include "GraphReconstruction/ControlRigGraphReconstruction.h"

#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FControlRigInstructionNodes, "AssetGenerator.Graphs.ControlRigInstructionNodes", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FControlRigInstructionNodes::RunTest(const FString& Parameters)
{
    FString Path, Text;
    if (!FParse::Value(FCommandLine::Get(), TEXT("ControlRigRecoveryJson="), Path)) { AddInfo(TEXT("Supply ControlRigRecoveryJson to reconstruct rig instruction nodes.")); return true; }
    TSharedPtr<FJsonObject> Json;
    if (!FFileHelper::LoadFileToString(Text, *Path) || !FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Text), Json)) { AddError(TEXT("Cannot read rig recovery JSON.")); return false; }
    const auto Data = Json->GetObjectField(TEXT("AssetSerializedData"))->GetObjectField(TEXT("AnimationBlueprintData"));
    for (const auto& Value : Data->GetArrayField(TEXT("ControlRigs")))
    {
        const auto Rig = Value->AsObject();
        auto Factory = NewObject<UControlRigBlueprintFactory>();
        FString SaveRoot;
        FParse::Value(FCommandLine::Get(), TEXT("RigRecoverySaveRoot="), SaveRoot);
        FString AssetName = Rig->GetStringField(TEXT("GeneratedClass"));
        AssetName.RemoveFromEnd(TEXT("_C"));
        UPackage* Package = UE427IsEmpty(SaveRoot) ? GetTransientPackage() : CreatePackage(*(SaveRoot / AssetName));
        const FName Name = UE427IsEmpty(SaveRoot) ? MakeUniqueObjectName(Package, UControlRigBlueprint::StaticClass(), TEXT("RecoveredRigAudit")) : FName(*AssetName);
        auto Blueprint = Cast<UControlRigBlueprint>(Factory->FactoryCreateNew(UControlRigBlueprint::StaticClass(), Package, Name, UE427IsEmpty(SaveRoot) ? RF_Transient : RF_Public | RF_Standalone, nullptr, GWarn));
        if (!Blueprint) { AddError(TEXT("Cannot create transient rig blueprint.")); return false; }
        TMap<int32, URigVMNode*> Nodes;
        TArray<FString> Errors;
        ControlRigGraphReconstruction::RestoreVariables(Blueprint, Rig, Errors);
        UObject* DefaultsObject = Blueprint->GeneratedClass ? Blueprint->GeneratedClass->GetDefaultObject() : nullptr;
        if (!TestNotNull(TEXT("Compiled rig defaults"), DefaultsObject)) return false;
        const auto SavedDefaults = Rig->GetObjectField(TEXT("Defaults"));
        for (const auto& VariableValue : Rig->GetArrayField(TEXT("PublicVariables")))
        {
            const FString VariableName = VariableValue->AsObject()->GetStringField(TEXT("Name"));
            FProperty* Property = DefaultsObject->GetClass()->FindPropertyByName(*VariableName);
            if (!TestNotNull(TEXT("Recovered variable: ") + VariableName, Property)) continue;
            const auto* Saved = SavedDefaults->Values.Find(VariableName);
            if (!Saved) continue;
            if (FStructProperty* StructProperty = CastField<FStructProperty>(Property))
            {
                if (StructProperty->Struct == TBaseStructure<FTransform>::Get())
                {
                    FTransform Expected;
                    TestTrue(TEXT("Read expected transform"), FJsonObjectConverter::JsonObjectToUStruct((*Saved)->AsObject().ToSharedRef(), StructProperty->Struct, &Expected, 0, 0));
                    const FTransform& Actual = *StructProperty->ContainerPtrToValuePtr<FTransform>(DefaultsObject);
                    TestTrue(TEXT("Compiled transform default: ") + VariableName, Actual.Equals(Expected, 0.001));
                }
                else if (StructProperty->Struct == TBaseStructure<FVector>::Get())
                {
                    FVector Expected;
                    TestTrue(TEXT("Read expected vector"), FJsonObjectConverter::JsonObjectToUStruct((*Saved)->AsObject().ToSharedRef(), StructProperty->Struct, &Expected, 0, 0));
                    const FVector& Actual = *StructProperty->ContainerPtrToValuePtr<FVector>(DefaultsObject);
                    TestTrue(TEXT("Compiled vector default: ") + VariableName, Actual.Equals(Expected, 0.001));
                }
            }
        }
        ControlRigGraphReconstruction::RestoreHierarchy(Blueprint, Rig, Errors);
        const auto& Elements = Rig->GetObjectField(TEXT("Hierarchy"))->GetArrayField(TEXT("Elements"));
        TestEqual(TEXT("All hierarchy elements restored"), Blueprint->Hierarchy->Num(), Elements.Num());
        for (const auto& ElementValue : Elements)
        {
            const auto Element = ElementValue->AsObject();
            const auto JsonKey = Element->GetObjectField(TEXT("LoadedKey"));
            const FRigElementKey Key(*JsonKey->GetStringField(TEXT("Name")), static_cast<ERigElementType>(JsonKey->GetIntegerField(TEXT("Type"))));
            if (Key.Type == ERigElementType::Curve) continue;
            const auto Local = Element->GetObjectField(TEXT("PoseStorage"))->GetObjectField(TEXT("Initial"))->GetObjectField(TEXT("Local"));
            if (Local->GetBoolField(TEXT("bDirty"))) continue;
            const auto Transform = Local->GetObjectField(TEXT("Transform"));
            const auto Position = Transform->GetObjectField(TEXT("Translation"));
            const auto Rotation = Transform->GetObjectField(TEXT("Rotation"));
            const auto Scale = Transform->GetObjectField(TEXT("Scale3D"));
            const FTransform Expected(FQuat(Rotation->GetNumberField(TEXT("X")), Rotation->GetNumberField(TEXT("Y")), Rotation->GetNumberField(TEXT("Z")), Rotation->GetNumberField(TEXT("W"))),
                FVector(Position->GetNumberField(TEXT("X")), Position->GetNumberField(TEXT("Y")), Position->GetNumberField(TEXT("Z"))),
                FVector(Scale->GetNumberField(TEXT("X")), Scale->GetNumberField(TEXT("Y")), Scale->GetNumberField(TEXT("Z"))));
            const FTransform Actual = Blueprint->Hierarchy->GetLocalTransform(Key, true);
            if (!TestTrue(TEXT("Restored initial pose: ") + Key.ToString(), Actual.Equals(Expected, 0.001)))
                AddInfo(TEXT("Expected ") + Expected.ToString() + TEXT("; actual ") + Actual.ToString());
        }
        ControlRigGraphReconstruction::CreateInstructionNodes(Blueprint, Rig, Nodes, Errors);
        ControlRigGraphReconstruction::RestoreDataLinks(Blueprint, Rig, Nodes, Errors);
        ControlRigGraphReconstruction::RestoreExecutionLinks(Blueprint, Rig, Nodes, Errors);
        if (UE427IsEmpty(Errors))
        {
            FKismetEditorUtilities::CompileBlueprint(Blueprint);
            Blueprint->RecompileVM();
            TestTrue(TEXT("Recovered rig compiles: ") + Rig->GetStringField(TEXT("GeneratedClass")), Blueprint->Status != BS_Error);
            const auto WorldSettings = UWorld::InitializationValues().AllowAudioPlayback(false).CreateNavigation(false).CreateAISystem(false);
            auto World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &WorldSettings);
            auto Instance = NewObject<UControlRig>(World, Blueprint->GeneratedClass);
            Instance->Initialize();
            Instance->SetDeltaTime(1.0f / 60.0f);
            TestTrue(TEXT("Recovered rig executes"), Instance->Execute(TEXT("Forwards Solve")));
            if (auto Trace = FindFProperty<FBoolProperty>(Instance->GetClass(), TEXT("ShouldDoIKTrace")))
            {
                for (bool Enabled : {true, false})
                {
                    Trace->SetPropertyValue_InContainer(Instance, Enabled);
                    for (int32 Frame = 0; Frame < 3; ++Frame)
                        TestTrue(TEXT("Recovered rig executes trace branch"), Instance->Execute(TEXT("Forwards Solve")));
                }
            }
            for (const auto& Key : Instance->GetHierarchy()->GetAllKeys())
                if (Key.Type != ERigElementType::Curve)
                    TestFalse(TEXT("Finite recovered pose: ") + Key.ToString(), Instance->GetHierarchy()->GetGlobalTransform(Key).ContainsNaN());
            AddInfo(FString::Printf(TEXT("Compiled %s: %d VM instructions."), *Rig->GetStringField(TEXT("GeneratedClass")), Instance->GetVM()->GetByteCode().GetNumInstructions()));
            if (!UE427IsEmpty(SaveRoot) && !HasAnyErrors())
            {
                FSavePackageArgs Args;
                Args.TopLevelFlags = RF_Public | RF_Standalone;
                const FString Filename = FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension());
                TestTrue(TEXT("Save recovered rig"), UPackage::SavePackage(Package, Blueprint, *Filename, Args));
            }
            World->DestroyWorld(false);
        }
        for (const FString& Error : Errors) AddError(Rig->GetStringField(TEXT("GeneratedClass")) + TEXT(": ") + Error);
        AddInfo(FString::Printf(TEXT("%s: recreated %d instruction nodes; %d errors."), *Rig->GetStringField(TEXT("GeneratedClass")), Nodes.Num(), Errors.Num()));
    }
    return !HasAnyErrors();
}
#endif

#endif
