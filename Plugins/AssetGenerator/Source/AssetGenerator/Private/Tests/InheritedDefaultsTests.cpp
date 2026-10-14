#include "CoreMinimal.h"
#if ENGINE_MAJOR_VERSION >= 5
#include "Misc/AutomationTest.h"
#include "UE427AssetPath.h"
#include "Toolkit/AssetTypeGenerator/BlueprintGenerator.h"
#include "Toolkit/ObjectHierarchySerializer.h"
#include "Toolkit/PropertySerializer.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "GameFramework/Actor.h"
#include "Curves/CurveFloat.h"
#include "EdGraphSchema_K2.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "UObject/UnrealType.h"

#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCASInheritedDefaultsTest, "AssetGenerator.Graphs.InheritedDefaults", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCASInheritedDefaultsTest::RunTest(const FString& Parameters) {
	auto Create = [](UClass* Parent) {
		return FKismetEditorUtilities::CreateBlueprint(Parent, GetTransientPackage(), MakeUniqueObjectName(GetTransientPackage(), UBlueprint::StaticClass(), TEXT("CASDefaultsTest")), BPTYPE_Normal, UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass());
	};
	auto Parent = Create(AActor::StaticClass());
	FEdGraphPinType Int;
	Int.PinCategory = UEdGraphSchema_K2::PC_Int;
	FBlueprintEditorUtils::AddMemberVariable(Parent, TEXT("InheritedValue"), Int, TEXT("101"));
	FBlueprintEditorUtils::AddMemberVariable(Parent, TEXT("UnmodifiedValue"), Int, TEXT("77"));
	FEdGraphPinType ObjectType;
	ObjectType.PinCategory = UEdGraphSchema_K2::PC_Object;
	ObjectType.PinSubCategoryObject = UObject::StaticClass();
	FBlueprintEditorUtils::AddMemberVariable(Parent, TEXT("NestedDefault"), ObjectType);
	FKismetEditorUtilities::CompileBlueprint(Parent, EBlueprintCompileOptions::SkipGarbageCollection);
	auto Child = Create(Parent->GeneratedClass);
	auto Value = [](UBlueprint* BP, const TCHAR* Name) {
		return FindFProperty<FIntProperty>(BP->GeneratedClass, Name)->GetPropertyValue_InContainer(BP->GeneratedClass->GetDefaultObject());
	};
	TestEqual(TEXT("Child initially inherits parent default"), Value(Child, TEXT("InheritedValue")), 101);
	auto Properties = MakeShared<FJsonObject>();
	Properties->SetNumberField(TEXT("InheritedValue"), 37);
	Properties->SetBoolField(TEXT("bAlwaysRelevant"), true);
	Properties->SetNumberField(TEXT("NestedDefault"), 1);
	auto Entry = MakeShared<FJsonObject>();
	Entry->SetObjectField(TEXT("Properties"), Properties);
	auto Serializer = NewObject<UObjectHierarchySerializer>();
	Serializer->SetPropertySerializer(NewObject<UPropertySerializer>());
	Entry->SetNumberField(TEXT("Outer"), INDEX_NONE);
	Entry->SetStringField(TEXT("Type"), TEXT("Export"));
	Entry->SetStringField(TEXT("ObjectName"), Child->GeneratedClass->GetDefaultObject()->GetName());
	Entry->SetNumberField(TEXT("ObjectClass"), 3);
	auto Nested = MakeShared<FJsonObject>();
	Nested->SetStringField(TEXT("Type"), TEXT("Export"));
	Nested->SetStringField(TEXT("ObjectName"), TEXT("NestedDefault"));
	Nested->SetNumberField(TEXT("ObjectClass"), 2);
	Nested->SetNumberField(TEXT("ObjectFlags"), RF_ArchetypeObject);
	Nested->SetNumberField(TEXT("Outer"), 0);
	auto ObjectClass = MakeShared<FJsonObject>();
	ObjectClass->SetStringField(TEXT("Type"), TEXT("Export"));
	ObjectClass->SetStringField(TEXT("ObjectMark"), TEXT("$NestedClass$"));
	Serializer->SetObjectMark(UCurveFloat::StaticClass(), TEXT("$NestedClass$"));
	Serializer->SetPackageForDeserialization(GetTransientPackage());
	auto BlueprintClass = MakeShared<FJsonObject>();
	BlueprintClass->SetStringField(TEXT("Type"), TEXT("Export"));
	BlueprintClass->SetStringField(TEXT("ObjectMark"), TEXT("$AssetObject$"));
	Serializer->InitializeForDeserialization({MakeShared<FJsonValueObject>(Entry), MakeShared<FJsonValueObject>(Nested), MakeShared<FJsonValueObject>(ObjectClass), MakeShared<FJsonValueObject>(BlueprintClass)});
	TestTrue(TEXT("Restore inherited overrides"), FBlueprintGeneratorUtils::RestoreClassDefaults(Child, Serializer, 0));
	auto NestedProperty = FindFProperty<FObjectProperty>(Child->GeneratedClass, TEXT("NestedDefault"));
	auto NestedObject = NestedProperty->GetObjectPropertyValue_InContainer(Child->GeneratedClass->GetDefaultObject());
	if (TestNotNull(TEXT("Nested default object restored"), NestedObject))
		TestTrue(TEXT("Nested object's outer is the existing CDO"), NestedObject->GetOuter() == Child->GeneratedClass->GetDefaultObject());
	TestTrue(TEXT("CDO export resolves without constructing an outer -1"), Serializer->DeserializeObject(0) == Child->GeneratedClass->GetDefaultObject());
	TestEqual(TEXT("Child override restored"), Value(Child, TEXT("InheritedValue")), 37);
	TestEqual(TEXT("Omitted property retains inheritance"), Value(Child, TEXT("UnmodifiedValue")), 77);
	TestEqual(TEXT("Parent remains unchanged"), Value(Parent, TEXT("InheritedValue")), 101);
	TestTrue(TEXT("Native inherited override restored"), CastChecked<AActor>(Child->GeneratedClass->GetDefaultObject())->bAlwaysRelevant);
	TestFalse(TEXT("Native parent default unchanged"), CastChecked<AActor>(Parent->GeneratedClass->GetDefaultObject())->bAlwaysRelevant);
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Child);
	FKismetEditorUtilities::CompileBlueprint(Child, EBlueprintCompileOptions::SkipGarbageCollection);
	TestEqual(TEXT("Override survives compilation"), Value(Child, TEXT("InheritedValue")), 37);
	TestTrue(TEXT("Native override survives compilation"), CastChecked<AActor>(Child->GeneratedClass->GetDefaultObject())->bAlwaysRelevant);
	TestFalse(TEXT("Repeated restoration is unchanged"), FBlueprintGeneratorUtils::RestoreClassDefaults(Child, Serializer, 0));
	auto Grandchild = Create(Child->GeneratedClass);
	TestEqual(TEXT("Grandchild inherits corrected child value"), Value(Grandchild, TEXT("InheritedValue")), 37);
	return true;
}
#endif

#endif
