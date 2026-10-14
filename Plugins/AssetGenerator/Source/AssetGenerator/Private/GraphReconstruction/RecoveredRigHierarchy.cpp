#include "CoreMinimal.h"
#if ENGINE_MAJOR_VERSION >= 5
#include "ControlRigGraphReconstruction.h"
#include "UE427AssetPath.h"
#include "ControlRigBlueprint.h"
#include "Rigs/RigHierarchy.h"
#include "Rigs/RigHierarchyController.h"
#include "JsonObjectConverter.h"
#include "Dom/JsonObject.h"

namespace ControlRigGraphReconstruction
{
namespace
{
FRigElementKey Key(const TSharedPtr<FJsonObject>& Json)
{
    return FRigElementKey(*Json->GetStringField(TEXT("Name")), static_cast<ERigElementType>(Json->GetIntegerField(TEXT("Type"))));
}

bool Transform(const TSharedPtr<FJsonObject>& Storage, bool Initial, FTransform& Result, bool& Global)
{
    const auto Phase = Storage->GetObjectField(Initial ? TEXT("Initial") : TEXT("Current"));
    auto Value = Phase->GetObjectField(TEXT("Local"));
    Global = Value->GetBoolField(TEXT("bDirty"));
    if (Global) Value = Phase->GetObjectField(TEXT("Global"));
    if (Value->GetBoolField(TEXT("bDirty"))) return false;
    const auto Json = Value->GetObjectField(TEXT("Transform"));
    const auto R = Json->GetObjectField(TEXT("Rotation"));
    const auto T = Json->GetObjectField(TEXT("Translation"));
    const auto S = Json->GetObjectField(TEXT("Scale3D"));
    FQuat Rotation(R->GetNumberField(TEXT("X")), R->GetNumberField(TEXT("Y")), R->GetNumberField(TEXT("Z")), R->GetNumberField(TEXT("W")));
    if (!Rotation.IsNormalized()) {
        const bool Uninitialized = Rotation.SizeSquared() == 0 &&
            T->GetNumberField(TEXT("X")) == 0 && T->GetNumberField(TEXT("Y")) == 0 && T->GetNumberField(TEXT("Z")) == 0 &&
            S->GetNumberField(TEXT("X")) == 0 && S->GetNumberField(TEXT("Y")) == 0 && S->GetNumberField(TEXT("Z")) == 0;
        return !Initial && Uninitialized && Transform(Storage, true, Result, Global);
    }
    Result = FTransform(Rotation, FVector(T->GetNumberField(TEXT("X")), T->GetNumberField(TEXT("Y")), T->GetNumberField(TEXT("Z"))),
        FVector(S->GetNumberField(TEXT("X")), S->GetNumberField(TEXT("Y")), S->GetNumberField(TEXT("Z"))));
    return !Result.ContainsNaN();
}
}

bool RestoreHierarchy(UControlRigBlueprint* Blueprint, const TSharedPtr<FJsonObject>& Data, TArray<FString>& Errors)
{
    URigHierarchy* Hierarchy = Blueprint->Hierarchy;
    URigHierarchyController* Controller = Blueprint->GetHierarchyController();
    if (!Hierarchy || !Controller) { Errors.Add(TEXT("Missing rig hierarchy controller.")); return false; }
    if (Hierarchy->Num() != 0) { Errors.Add(TEXT("Rig hierarchy recovery requires an empty target.")); return false; }
    const auto& Elements = Data->GetObjectField(TEXT("Hierarchy"))->GetArrayField(TEXT("Elements"));
    for (const auto& Value : Elements)
    {
        const auto Element = Value->AsObject();
        const auto ElementKey = Key(Element->GetObjectField(TEXT("LoadedKey")));
        FRigElementKey Added;
        switch (ElementKey.Type)
        {
        case ERigElementType::Bone:
            Added = Controller->AddBone(ElementKey.Name, {}, FTransform::Identity, false, static_cast<ERigBoneType>(Element->GetIntegerField(TEXT("BoneType"))), false);
            break;
        case ERigElementType::Null:
            Added = Controller->AddNull(ElementKey.Name, {}, FTransform::Identity, false, false);
            break;
        case ERigElementType::Curve:
            Added = Controller->AddCurve(ElementKey.Name, Element->GetNumberField(TEXT("Value")), false);
            if (!Element->GetBoolField(TEXT("bIsValueSet"))) Hierarchy->UnsetCurveValue(Added);
            break;
        case ERigElementType::Control:
        {
            if (!Element->HasTypedField<EJson::Object>(TEXT("Settings"))) { Errors.Add(TEXT("Control settings missing: ") + ElementKey.Name.ToString()); continue; }
            auto SettingsJson = MakeShared<FJsonObject>();
            SettingsJson->Values = Element->GetObjectField(TEXT("Settings"))->Values;
            for (const TCHAR* Field : {TEXT("MinimumValue"), TEXT("MaximumValue")})
            {
                auto Storage = MakeShared<FJsonObject>();
                Storage->Values = SettingsJson->GetObjectField(Field)->Values;
                Storage->SetBoolField(TEXT("bValid"), true);
                auto Wrapped = MakeShared<FJsonObject>();
                Wrapped->SetObjectField(TEXT("FloatStorage"), Storage);
                SettingsJson->SetObjectField(Field, Wrapped);
            }
            FRigControlSettings Settings;
            if (!FJsonObjectConverter::JsonObjectToUStruct(SettingsJson, &Settings)) { Errors.Add(TEXT("Invalid control settings: ") + ElementKey.Name.ToString()); continue; }
            Added = Controller->AddControl(ElementKey.Name, {}, Settings, FRigControlValue(), FTransform::Identity, FTransform::Identity, false);
            break;
        }
        default: Errors.Add(TEXT("Unsupported hierarchy element: ") + ElementKey.ToString()); continue;
        }
        if (Added != ElementKey) Errors.Add(TEXT("Hierarchy element could not retain its key: ") + ElementKey.ToString());
    }
    if (!UE427IsEmpty(Errors)) return false;
    for (const auto& Value : Elements)
    {
        const auto Element = Value->AsObject();
        const auto Child = Key(Element->GetObjectField(TEXT("LoadedKey")));
        if (Element->HasTypedField<EJson::Object>(TEXT("ParentKey")))
        {
            const auto Parent = Key(Element->GetObjectField(TEXT("ParentKey")));
            if (Parent.IsValid() && !Controller->AddParent(Child, Parent, 1.f, false, false)) Errors.Add(TEXT("Cannot restore parent of ") + Child.ToString());
        }
        const TArray<TSharedPtr<FJsonValue>>* Parents;
        if (Element->TryGetArrayField(TEXT("ParentConstraints"), Parents))
        {
            for (const auto& ParentValue : *Parents)
            {
                const auto Constraint = ParentValue->AsObject();
                const auto Parent = Key(Constraint->GetObjectField(TEXT("ParentKey")));
                if (!Controller->AddParent(Child, Parent, 0.f, false, false)) { Errors.Add(TEXT("Cannot restore control parent of ") + Child.ToString()); continue; }
                for (bool Initial : {true, false})
                {
                    FRigElementWeight Weight;
                    if (!FJsonObjectConverter::JsonObjectToUStruct(Constraint->GetObjectField(Initial ? TEXT("InitialWeight") : TEXT("Weight")).ToSharedRef(), &Weight)
                        || !Hierarchy->SetParentWeight(Child, Parent, Weight, Initial, false)) Errors.Add(TEXT("Cannot restore parent weights of ") + Child.ToString());
                }
            }
        }
    }
    if (!UE427IsEmpty(Errors)) return false;
    for (const auto& Value : Elements)
    {
        const auto Element = Value->AsObject();
        const auto ElementKey = Key(Element->GetObjectField(TEXT("LoadedKey")));
        if (ElementKey.Type == ERigElementType::Curve) continue;
        for (bool Initial : {true, false})
        {
            if (ElementKey.Type == ERigElementType::Control)
            {
                for (const TCHAR* Field : {TEXT("Offset"), TEXT("Shape")})
                {
                    FTransform Local;
                    bool Global;
                    if (!Transform(Element->GetObjectField(Field), Initial, Local, Global) || Global) { Errors.Add(TEXT("Missing valid local control transform: ") + ElementKey.ToString() + TEXT(".") + Field); continue; }
                    if (FCString::Strcmp(Field, TEXT("Offset")) == 0) Hierarchy->SetControlOffsetTransform(ElementKey, Local, Initial, true);
                    else Hierarchy->SetControlShapeTransform(ElementKey, Local, Initial);
                }
            }
            FTransform Pose;
            bool Global;
            if (!Transform(Element->GetObjectField(TEXT("PoseStorage")), Initial, Pose, Global)) { Errors.Add(TEXT("Missing valid pose transform: ") + ElementKey.ToString()); continue; }
            if (Global) Hierarchy->SetGlobalTransform(ElementKey, Pose, Initial, true);
            else Hierarchy->SetLocalTransform(ElementKey, Pose, Initial, true);
        }
        if (ElementKey.Type == ERigElementType::Control && Element->HasTypedField<EJson::Object>(TEXT("PreferredEulerAngles")))
        {
            auto Control = Hierarchy->Find<FRigControlElement>(ElementKey);
            if (!FJsonObjectConverter::JsonObjectToUStruct(Element->GetObjectField(TEXT("PreferredEulerAngles")).ToSharedRef(), &Control->PreferredEulerAngles))
                Errors.Add(TEXT("Cannot restore preferred Euler angles: ") + ElementKey.ToString());
        }
    }
    return UE427IsEmpty(Errors);
}
}

#endif
