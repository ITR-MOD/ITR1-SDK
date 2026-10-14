#include "CoreMinimal.h"
#if ENGINE_MAJOR_VERSION >= 5
#include "ControlRigGraphReconstruction.h"
#include "UE427AssetPath.h"
#include "ControlRigBlueprint.h"
#include "Dom/JsonObject.h"
#include "JsonObjectConverter.h"
#include "UObject/StructOnScope.h"
#include "UObject/UnrealType.h"

namespace ControlRigGraphReconstruction
{
bool RestoreVariables(UControlRigBlueprint* Blueprint, const TSharedPtr<FJsonObject>& Data, TArray<FString>& Errors)
{
    const auto Defaults = Data->GetObjectField(TEXT("Defaults"));
    for (const auto& Value : Data->GetArrayField(TEXT("PublicVariables")))
    {
        const auto Variable = Value->AsObject();
        const FString Name = Variable->GetStringField(TEXT("Name"));
        const FString Type = Variable->GetStringField(TEXT("Type"));
        FString CPPType, Default;
        const auto* SavedDefault = Defaults->Values.Find(Name);
        if (Type == TEXT("ArrayProperty") && Variable->GetObjectField(TEXT("Inner"))->GetStringField(TEXT("Type")) == TEXT("StructProperty"))
        {
            const auto Reference = Variable->GetObjectField(TEXT("Inner"))->GetObjectField(TEXT("Struct"));
            FString Prefix, StructName;
            if (!Reference->GetStringField(TEXT("ObjectName")).Split(TEXT("'"), &Prefix, &StructName)) { Errors.Add(TEXT("Invalid rig array struct: ") + Name); continue; }
            StructName.RemoveFromEnd(TEXT("'"));
            const FString Path = Reference->GetStringField(TEXT("ObjectPath")) + TEXT(".") + StructName;
            UScriptStruct* Struct = LoadObject<UScriptStruct>(nullptr, *Path);
            if (!Struct) { Errors.Add(TEXT("Missing rig array struct: ") + Path); continue; }
            CPPType = TEXT("TArray<") + Path + TEXT(">");
            Default = TEXT("(");
            if (SavedDefault)
            {
                if ((*SavedDefault)->Type != EJson::Array) { Errors.Add(TEXT("Invalid rig array default: ") + Name); continue; }
                bool bFirst = true;
                for (const auto& Item : (*SavedDefault)->AsArray())
                {
                    FStructOnScope Storage(Struct);
                    if (Item->Type != EJson::Object || !FJsonObjectConverter::JsonObjectToUStruct(Item->AsObject().ToSharedRef(), Struct, Storage.GetStructMemory(), 0, 0))
                    { Errors.Add(TEXT("Cannot restore rig array element: ") + Name); break; }
                    FString Element;
                    Struct->ExportText(Element, Storage.GetStructMemory(), nullptr, nullptr, PPF_None, nullptr);
                    if (!bFirst) Default += TEXT(",");
                    Default += Element;
                    bFirst = false;
                }
            }
            Default += TEXT(")");
        }
        else if (Type == TEXT("StructProperty"))
        {
            const auto Reference = Variable->GetObjectField(TEXT("Struct"));
            FString Prefix, StructName;
            if (!Reference->GetStringField(TEXT("ObjectName")).Split(TEXT("'"), &Prefix, &StructName)) { Errors.Add(TEXT("Invalid rig variable struct: ") + Name); continue; }
            StructName.RemoveFromEnd(TEXT("'"));
            const FString Path = Reference->GetStringField(TEXT("ObjectPath")) + TEXT(".") + StructName;
            UScriptStruct* Struct = LoadObject<UScriptStruct>(nullptr, *Path);
            if (!Struct) { Errors.Add(TEXT("Missing rig variable struct: ") + Path); continue; }
            CPPType = Path;
            FStructOnScope Storage(Struct);
            if (SavedDefault && ((*SavedDefault)->Type != EJson::Object || !FJsonObjectConverter::JsonObjectToUStruct((*SavedDefault)->AsObject().ToSharedRef(), Struct, Storage.GetStructMemory(), 0, 0)))
            { Errors.Add(TEXT("Cannot restore rig variable default: ") + Name); continue; }
            if (Struct == TBaseStructure<FTransform>::Get())
                Default = reinterpret_cast<const FTransform*>(Storage.GetStructMemory())->ToString();
            else if (Struct == TBaseStructure<FVector>::Get())
            {
                const FVector& Vector = *reinterpret_cast<const FVector*>(Storage.GetStructMemory());
                Default = FString::Printf(TEXT("%.17g,%.17g,%.17g"), Vector.X, Vector.Y, Vector.Z);
            }
            else
                Struct->ExportText(Default, Storage.GetStructMemory(), nullptr, nullptr, PPF_None, nullptr);
        }
        else if (Type == TEXT("DoubleProperty") || Type == TEXT("FloatProperty") || Type == TEXT("IntProperty"))
        {
            CPPType = Type == TEXT("DoubleProperty") ? TEXT("double") : Type == TEXT("FloatProperty") ? TEXT("float") : TEXT("int32");
            Default = FString::Printf(TEXT("%.17g"), SavedDefault ? (*SavedDefault)->AsNumber() : 0.0);
        }
        else if (Type == TEXT("BoolProperty"))
        {
            CPPType = TEXT("bool");
            Default = SavedDefault && (*SavedDefault)->AsBool() ? TEXT("True") : TEXT("False");
        }
        else { Errors.Add(TEXT("Unsupported rig variable type: ") + Type + TEXT(" ") + Name); continue; }
        FString Flags;
        Variable->TryGetStringField(TEXT("PropertyFlags"), Flags);
        if (Blueprint->AddMemberVariable(*Name, CPPType, Flags.Contains(TEXT("BlueprintVisible")), Flags.Contains(TEXT("BlueprintReadOnly")), Default) != FName(*Name))
            Errors.Add(TEXT("Could not retain rig variable name: ") + Name);
    }
    return UE427IsEmpty(Errors);
}
}

#endif
